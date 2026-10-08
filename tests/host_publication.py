#!/usr/bin/env python3
"""Replay actual host admission: no PCI reads before successful publication."""
from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parents[1]
s=(r/'WhateverXe/kern_host.cpp').read_text();body=s[s.index('void XeHost::processKernel'):s.index('void XeHost::processKext')]
code=r'''
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
std::vector<int> events;bool publish=true,published=false;uint32_t identity=0x9a498086;
struct IOPCIDevice {
 uint32_t configRead32(int){assert(published);events.push_back(2);return identity;}
 uint8_t configRead8(int){assert(published);events.push_back(3);return 1;}
 uint16_t configRead16(int){assert(published);events.push_back(4);return 0x300;}
 const char *getName(){return "GFX0";}void *getProperty(const char *){return nullptr;}
 void setProperty(const char *,const void *,unsigned){}void setProperty(const char *,uint64_t,unsigned){}
};
#define OSDynamicCast(type,p) (p)
struct DeviceInfo {
 enum class FirmwareVendor{Apple,Other};FirmwareVendor firmwareVendor=FirmwareVendor::Other;
 IOPCIDevice *videoBuiltin=nullptr;bool reportedFramebufferIsConnectorLess=false;
 static DeviceInfo value;static DeviceInfo *create(){return &value;}static void deleter(DeviceInfo *){events.push_back(9);}
};DeviceInfo DeviceInfo::value;
namespace WIOKit {bool awaitPublishing(IOPCIDevice *){events.push_back(1);published=publish;return publish;}void renameDevice(IOPCIDevice *,const char *){events.push_back(5);}}
namespace XELP {struct Profile{bool yes;bool admitted(){return yes;}};Profile identify(uint32_t id,uint8_t){return {id==0x9a498086};}}
struct KernelPatcher {static constexpr int KernelID=0;uintptr_t solveSymbol(int,const char *){return 0;}void clearError(){}};
struct Graphics {bool off=false;void switchOff(){off=true;}} graphics;
struct XeHost {
 bool active=false;enum {FB_DETECT,FB_RESET,FB_COPY,FB_ZEROFILL,FB_NONE,FB_TOTAL};uint32_t resetFramebuffer=FB_NONE;
 struct vc_info{};vc_info consoleVinfo;bool gotConsoleVinfo=false;
 struct XE {void processKernel(KernelPatcher &,DeviceInfo *){events.push_back(8);}} xe;
 void processKernel(KernelPatcher &);
};
'''+body+r'''
int main(){KernelPatcher patcher;IOPCIDevice gpu;
for(unsigned test=0;test<4;++test){
 events.clear();published=false;publish=test!=1;identity=test==2?0x12348086:0x9a498086;graphics.off=false;
 DeviceInfo::value.videoBuiltin=test==0?nullptr:&gpu;XeHost host;host.processKernel(patcher);
 assert(events.back()==9);
 if(test<2){assert(!host.active&&graphics.off);assert(events.size()==(test==0?1:2));}
 else if(test==2){assert(!host.active&&graphics.off);assert(events.size()==4);}
 else {assert(host.active&&!graphics.off);assert(events.front()==1);assert(events[events.size()-2]==8);}
}
puts("PASS actual host admission: absent/unpublished/unsupported GPUs never enter Xe; published PCI reads precede selected-driver dispatch");}
'''
with tempfile.TemporaryDirectory(prefix='wxe-host-') as d:
 p=Path(d);(p/'test.cpp').write_text(code)
 subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
