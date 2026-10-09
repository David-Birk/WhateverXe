#!/usr/bin/env python3
"""Exercise actual plugin/provider startup selection with new and obsolete args."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
xe = (root / 'WhateverXe/kern_xe.cpp').read_text()
provider = (root / 'AppleIntelTGLCompat/AppleIntelTGLCompat.cpp').read_text()
start = (root / 'WhateverXe/kern_start.cpp').read_text()
# Guard the public namespace across every reader, including plugin arrays.
external = {'-igfxdvmt', 'gfxrst', '-allow3d', '-disablegfxfirmware'}
used = set()
for directory in ['WhateverXe', 'AppleIntelTGLCompat']:
    for path in (root / directory).glob('*.cpp'):
        used.update(re.findall(r'(?:checkKernelArgument|PE_parse_boot_argn)\("([^"]+)"', path.read_text()))
for array, expected in [('Off', '-wexoff'), ('Debug', '-wexdbg'), ('Beta', '-wexbeta')]:
    body = re.search(r'bootarg' + array + r'\[\] \{(.*?)\}', start, re.S).group(1)
    assert re.findall(r'"([^"]+)"', body) == [expected]
    used.add(expected)
assert external <= used
assert all(name.startswith('-wex') or name in external for name in used), used
assert not (root / 'WhateverXe/kern_boot_args.hpp').exists()
guide = (root / 'docs/boot-arguments.md').read_text()
rows = re.findall(r'^\| `([^`]+)` \| (.*?) \|$', guide, re.M)
assert {name for name, _ in rows} == used - external
removed = [name for _, text in rows for name in re.findall(r'`([^`]+)`', text)]
assert not set(removed) & used
init = xe[xe.index('void XE::init() {'):xe.index('void XE::deinit()')]
entry = re.search(r'extern "C" kern_return_t tglStart\(.*?\n\}', provider, re.S).group()
fixture = r'''
#include <cassert>
#include <cstring>
#include <set>
#include <string>
#include <initializer_list>
static const char *qualified = "Darwin Kernel Version 20.6.0: Thu Jul  6 22:12:47 PDT 2023; root:xnu-7195.141.49.702.12~1/RELEASE_X86_64";
static const char *version = qualified;
static std::set<std::string> args;
bool checkKernelArgument(const char *name) { return args.count(name); }
bool PE_parse_boot_argn(const char *name, void *, unsigned) { return checkKernelArgument(name); }
#define SYSLOG(...) ((void)0)
#define IOLog(...) ((void)0)
namespace CPUInfo { enum { CPU_MODEL_TIGERLAKE_U = 1 }; }
struct BaseDeviceInfo {
 int cpuModel = 0;
 static BaseDeviceInfo &get() { static BaseDeviceInfo info; return info; }
};
static int kextIntelTGLFb, kextAppleIntelTGLCompat, kextIntelTGL;
static struct { void onKextLoadForce(int *) {} } lilu;
struct XE {
 bool adlnRequested=false, adlnKernelReady=false, adlnNativeRequested=false;
 bool display13FramebufferRequested=false, display13TraceEnabled=false;
 bool tglRequested=false, tglAcceleratorRequested=false, tglNoPanel=false;
 bool tglInactivePorts=false, tglPG1Poll=false, tglClockOrderRequested=false;
 bool tglProgramEnabled=false, tglTrainingLeaseRequested=false;
 struct { bool available=false; } modDVMTCalcFix;
 int *currentGraphics=nullptr, *currentFramebuffer=nullptr;
 void init();
};
static XE *callbackXE;
using kern_return_t = int;
struct kmod_info_t {};
static constexpr int KERN_FAILURE=5, KERN_SUCCESS=0;
static bool adlnRequested, hardwareEnabled;
// ACTUAL_SOURCE
static XE boot(std::initializer_list<const char *> names, int model=0) {
 args.clear(); for (const auto name:names) args.insert(name);
 BaseDeviceInfo::get().cpuModel=model;
 XE xe; xe.init(); assert(tglStart(nullptr,nullptr)==KERN_SUCCESS);
 assert(xe.adlnRequested==adlnRequested);
 assert(hardwareEnabled==!adlnRequested);
 return xe;
}
int main() {
 auto xe=boot({"-wex-adln","-wex-display13","-wex-adln-native","-wex-adln-rcs-profile"});
 assert(xe.adlnRequested && xe.adlnKernelReady && xe.adlnNativeRequested && xe.display13FramebufferRequested);
 assert(xe.tglAcceleratorRequested && !xe.display13TraceEnabled);
 xe=boot({"-wex-adln","-wex-display13","-wex-display13-trace"});
 assert(xe.display13TraceEnabled && !xe.adlnNativeRequested);
 xe=boot({"-wex-adln","-n97native","-adlnnative","-n97display13","-display13"});
 assert(!xe.adlnNativeRequested && !xe.display13FramebufferRequested);
 for(const char *obsolete:{"-igfxn97","-igfxadln"}) {
  xe=boot({obsolete,"-wex-adln-native","-wex-display13"});
  assert(!xe.adlnRequested && !xe.tglRequested && !xe.adlnNativeRequested);
 }
 xe=boot({},CPUInfo::CPU_MODEL_TIGERLAKE_U);
 assert(xe.tglRequested && xe.tglAcceleratorRequested && xe.tglProgramEnabled && !xe.tglNoPanel);
 xe=boot({"-wex-tgl","-wex-nopanel","-wex-tgl-prog-off"});
 assert(xe.tglRequested && xe.tglNoPanel && !xe.tglProgramEnabled);
 xe=boot({"-wex-tgl-off"},CPUInfo::CPU_MODEL_TIGERLAKE_U);
 assert(!xe.tglRequested);
 xe=boot({"-igfxtgl","-tglnopanel","-tglprogoff"});
 assert(!xe.tglRequested && !xe.tglNoPanel && xe.tglProgramEnabled);
 xe=boot({"-igfxtgloff"},CPUInfo::CPU_MODEL_TIGERLAKE_U);
 assert(xe.tglRequested);
 args={"-wex-adln","-wex-display13","-wex-adln-native"};
 version="unqualified"; xe=XE{}; xe.init();
 assert(xe.adlnRequested && !xe.adlnKernelReady && !xe.adlnNativeRequested && !xe.display13FramebufferRequested);
 assert(tglStart(nullptr,nullptr)==KERN_FAILURE);
}
'''.replace('// ACTUAL_SOURCE', init + entry)
with tempfile.TemporaryDirectory(prefix='wex-boot-args-') as directory:
    directory = Path(directory)
    cpp, exe = directory / 'test.cpp', directory / 'test'
    cpp.write_text(fixture)
    subprocess.run(['clang++', '-std=c++17', '-O1', '-g', '-fsanitize=address,undefined',
                    '-fno-omit-frame-pointer', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print(f'boot namespace: {len(used-external)} options, {len(removed)} removed spellings; plugin/provider startup passed')
