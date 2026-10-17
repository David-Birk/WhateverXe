// ADL-P (46A8) admission, GT2 resources and Display13 VBT port mapping.
#include "kern_alderlake_n.hpp"
#include "kern_vbt.hpp"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace XELP;
int main() {
    // Only the reviewed C0/D0 stepping is admitted; other ADL-P IDs stay catalogued.
    for(unsigned rev=0;rev<256;++rev) {
        const auto p=identify(0x46A88086,rev);
        assert(p.family==Family::AlderLakeP && p.display==Display::XeLPD && p.gt==GT::ADLPGT2);
        assert(p.admitted()==(rev==0x0C));
        assert(AlderLakeN::matches(0x46A88086,rev)==(rev==0x0C));
    }
    for(uint16_t id:{0x46A3,0x46A6,0x46AA,0x46B3,0x46C3})
        assert(!identify((uint32_t(id)<<16)|0x8086,0x0C).admitted());
    assert(display13Family(Family::AlderLakeN) && display13Family(Family::AlderLakeP));
    assert(!display13Family(Family::TigerLake) && !display13Family(Family::RaptorLakeP));

    // GT2: five DSS (80 EU, T14 G3) or six (96 EU), all EUs and eight L3 banks.
    AlderLakeN::Resources r;
    assert(AlderLakeN::resources(1,0x1F,0,0,r,GT::ADLPGT2));
    assert(r.dss==5 && r.nativeSubslices==10 && r.eus==80 && r.l3Banks==8);
    assert(AlderLakeN::resources(1,0x3F,0,0,r,GT::ADLPGT2) && r.eus==96 && r.nativeSubslices==12);
    assert(!AlderLakeN::resources(1,0x0F,0,0,r,GT::ADLPGT2));    // GT1-sized
    assert(!AlderLakeN::resources(1,0x1F,0x01,0,r,GT::ADLPGT2)); // partial DSS
    assert(!AlderLakeN::resources(1,0x1F,0,0xF0,r,GT::ADLPGT2)); // ADL-N L3
    assert(!AlderLakeN::resources(2,0x1F,0,0,r,GT::ADLPGT2) && !AlderLakeN::resources(1,0x7F,0,0,r,GT::ADLPGT2));
    // The ADL-N profile is unchanged by the default argument.
    assert(AlderLakeN::resources(1,3,0x88,0xF0,r) && r.eus==24);
    assert(!AlderLakeN::resources(1,0x1F,0,0,r));
    assert(AlderLakeN::vdboxVebox(GT::ADLNSmall)==0x000E00FE && AlderLakeN::vdboxVebox(GT::ADLPGT2)==0x000E00FA);

    // T14 G3 VBT: eDP DVO 10 (A), HDMI DVO 7 (B), TC1..4 DVO 13/15/17/19.
    assert(VBT::ddi(Family::AlderLakeP,10)==0 && VBT::ddi(Family::AlderLakeP,7)==1);
    assert(VBT::ddi(Family::AlderLakeP,8)==PortCount && VBT::ddi(Family::AlderLakeP,9)==PortCount);
    const uint8_t tc[]={13,15,17,19};
    for(unsigned i=0;i<4;++i)assert(VBT::ddi(Family::AlderLakeP,tc[i])==3+i);
    for(uint32_t dvo=0;dvo<32;++dvo)assert(VBT::ddi(Family::AlderLakeP,dvo)==VBT::ddi(Family::AlderLakeN,dvo));
    puts("PASS AlderLakeP: 46A8 rev 0x0C admission, GT2 80/96-EU resources, Display13 VBT ports.");
}
