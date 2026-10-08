#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include "AppleIntelTGLCompatPortProbe.hpp"
struct Meta {unsigned size=0x1C20;unsigned getClassSize(){return size;}};
struct OSObject {
 virtual ~OSObject()=default;
 alignas(8) uint8_t storage[0x1C20] {};
 Meta meta;unsigned retains=0;
 Meta *getMetaClass(){return &meta;}
 void retain(){++retains;}
};
struct IOService:OSObject {OSObject *provider {};OSObject *getProvider(){return provider;}};
struct IOPCIDevice:OSObject {uint32_t identity {};uint8_t revision {};
 uint32_t configRead32(unsigned r){assert(!r);return identity;}
 uint8_t configRead8(unsigned r){assert(r==8);return revision;}
};
struct IOMemoryMap:OSObject {uintptr_t address {};uint64_t bytes {};
 uintptr_t getVirtualAddress(){return address;}uint64_t getLength(){return bytes;}
};
#define OSDynamicCast(T,p) dynamic_cast<T *>(p)
#define IOLog(...) ((void)0)
// PROVIDER_SOURCE
int main(int argc,char **argv) {
 assert(argc==2);const bool adln=!strcmp(argv[1],"adln");
 IOService controller;IOPCIDevice pci;OSObject access,port,foreign;IOMemoryMap map;
 std::vector<uint8_t> mmio(0x1000000);alignas(8) uint8_t platform[0x1C0] {};
 pci.identity=adln?0x46D18086:0x9A498086;controller.provider=&pci;platform[0x12]=9;
 map.address=reinterpret_cast<uintptr_t>(mmio.data());map.bytes=mmio.size();
 field<void *>(&controller,0xC40)=&access;field<void *>(&controller,0xD40)=platform;
 field<OSObject *>(&controller,0xC70)=&map;field<uintptr_t>(&access,0x50)=map.address;
 field<uint32_t>(&access,0x60)=map.bytes;access.meta.size=0x68;
 port.meta.size=0x6B0;field<void *>(&port,0x578)=&access;
 adlnRequested=adln;hardwareEnabled=!adln;
 XELP::ProbeConfig config;config.profile=XELP::identify(pci.identity,0);
 config.combo=3;config.comboBackend=adln?3:7;config.legacy=adln?0:0x1F8;config.legacyBackend=adln?0:0x1F8;
 // The new contract derives generation from physical identity, not boot args.
 auto malformed=config;malformed.abi=1;assert(!xelpConfigureDisplay(&controller,&malformed));
 malformed=config;malformed.bytes--;assert(!xelpConfigureDisplay(&controller,&malformed));
 malformed=config;malformed.profile.display=XELP::Display::Unknown;assert(!xelpConfigureDisplay(&controller,&malformed));
 pci.identity^=0x10000;assert(!xelpConfigureDisplay(&controller,&config));pci.identity^=0x10000;
 map.address++;assert(!xelpConfigureDisplay(&controller,&config));map.address--;
 assert(!controller.retains && !access.retains && !map.retains && !probeBindingState);
 assert(xelpConfigureDisplay(&controller,&config));
 assert(controller.retains==1 && access.retains==1 && map.retains==1);
 assert(xelpConfigureDisplay(&controller,&config));assert(controller.retains==1);
 malformed=config;malformed.combo=1;assert(!xelpConfigureDisplay(&controller,&malformed));
 if(adln){assert(xelpConfigurePortProbe(&controller,1,3,3));assert(!xelpConfigurePortProbe(&controller,2,3,3));}
 for(unsigned d=0;d<9;++d){auto rec=reinterpret_cast<uint32_t *>(platform+0x20+24*d);
  const uint32_t values[]={d,d+1,2,0,0x800,0x202};memcpy(rec,values,sizeof(values));
  field<void *>(&port,0x548)=rec;
  if(d>=3){*reinterpret_cast<uint32_t *>(mmio.data()+0xC4000)=1U<<(24+d-3);
   *reinterpret_cast<uint32_t *>(mmio.data()+0x1638A0)=0x10;
   const uint32_t bases[]={0x163000,0x16E000,0x16F000};const auto base=bases[(d-3)/2];
   *reinterpret_cast<uint32_t *>(mmio.data()+base+0x8A0)=0x10;
   *reinterpret_cast<uint32_t *>(mmio.data()+base+0x890)=1U<<((d-3)%2);}
  assert(tglProbePort(&port)==(adln?(d<2?1U:0U):(d<3?1U:4U)));
  field<void *>(&port,0x578)=&foreign;assert(!tglProbePort(&port));field<void *>(&port,0x578)=&access;
  field<void *>(&port,0x548)=reinterpret_cast<void *>(1);assert(!tglProbePort(&port));
 }
 // No second ownership epoch, no rebinding to a changed platform.
 field<void *>(&controller,0xD40)=nullptr;assert(!xelpConfigureDisplay(&controller,&config));
 assert(controller.retains==1 && access.retains==1 && map.retains==1);
 printf("PASS actual %s provider entries: ABI/profile/map validation, publish-once pinning, idempotence, foreign/stale binding rejection and native port probe. CPU only.\n",argv[1]);
}
