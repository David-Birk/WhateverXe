// Shared WEG/provider CPU contract. Recognition is not runtime qualification.
// Keep hardware capability, donor ABI and board transport as separate inputs.
#ifndef KERN_XELP_PLATFORM_HPP
#define KERN_XELP_PLATFORM_HPP
#include <stdint.h>
#include <stddef.h>
namespace XELP {
enum class Family : uint32_t { Unknown, TigerLake, AlderLakeN };
enum class Display : uint32_t { Unknown, XeD = 12, XeLPD = 13 };
enum class GT : uint32_t { Unknown, TGLGT2, TGLG4, ADLNSmall };
enum class Donor : uint32_t { Unknown, C004FC1A = 1 };
struct Profile {
    uint32_t identity {}, revision {};
    Family family {Family::Unknown};
    Display display {Display::Unknown};
    GT gt {GT::Unknown};
    Donor donor {Donor::Unknown};
    bool admitted() const {return family!=Family::Unknown && donor==Donor::C004FC1A &&
        (family!=Family::AlderLakeN || revision==0);}
};
inline Profile identify(uint32_t pci,uint32_t revision) {
    Profile p;p.identity=pci;p.revision=revision;
    if((pci&0xFFFF)!=0x8086 || revision>255)return p;
    const auto id=pci>>16;
    if(id==0x9A49 || id==0x9A40 || id==0x9A78) {
        p.family=Family::TigerLake;p.display=Display::XeD;
        p.gt=id==0x9A78 ? GT::TGLG4 : GT::TGLGT2;
    } else if(id>=0x46D0 && id<=0x46D4) {
        p.family=Family::AlderLakeN;p.display=Display::XeLPD;p.gt=GT::ADLNSmall;
    } else return p;
    p.donor=Donor::C004FC1A;return p;
}
inline bool same(const Profile &a,const Profile &b) {
    return a.identity==b.identity && a.revision==b.revision && a.family==b.family &&
        a.display==b.display && a.gt==b.gt && a.donor==b.donor;
}
constexpr uint32_t PortCount=9,PortMask=(1U<<PortCount)-1,PipeCount=3;
// A validated native association. The adapter validates transport-specific
// record contents before construction; the core does not infer HDMI or DP.
struct NativePorts {
    const void *platform {};
    uint32_t selected {};
    bool valid() const {return platform && selected && !(selected&~PortMask);}
    bool bound(const void *config,uint32_t ddi) const {
        return valid() && ddi<PortCount && (selected&(1U<<ddi)) &&
            config==static_cast<const uint8_t *>(platform)+0x20+24*ddi;
    }
};
// ABI2 binds a profile and immutable board intent, not mutable connection state.
// Type-C intent is represented but not admitted by either current backend.
struct ProbeConfig {
    uint32_t abi {2},bytes {sizeof(ProbeConfig)};
    Profile profile;
    uint32_t combo {},legacy {},typeC {},excluded {},comboBackend {},legacyBackend {};
    bool valid() const {
        if(abi!=2 || bytes!=sizeof(ProbeConfig) || !profile.admitted() ||
           !same(profile,identify(profile.identity,profile.revision)))return false;
        if((combo&~7U) || ((legacy|typeC|excluded|legacyBackend)&~0x1F8U) ||
           (comboBackend&~7U) || (legacy&typeC) || (legacy&excluded) || (typeC&excluded))return false;
        if(profile.display==Display::XeD)return comboBackend==7;
        return profile.display==Display::XeLPD && combo && !(combo&~3U) &&
            comboBackend && !(comboBackend&~3U) && !legacy && !typeC && !legacyBackend;
    }
};
inline bool same(const ProbeConfig &a,const ProbeConfig &b) {
    return a.abi==b.abi && a.bytes==b.bytes && same(a.profile,b.profile) && a.combo==b.combo &&
        a.legacy==b.legacy && a.typeC==b.typeC && a.excluded==b.excluded &&
        a.comboBackend==b.comboBackend && a.legacyBackend==b.legacyBackend;
}
using ConfigureProbe = bool (*)(void *,const ProbeConfig *);
using ProbePort = uint32_t (*)(void *);
}
#endif
