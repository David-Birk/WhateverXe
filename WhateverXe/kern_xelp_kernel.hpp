// Exact kernels a donor path is qualified on. Identity is the full kernel
// version string; there are no version ranges. A path admits a kernel only
// after its private kernel assumptions were checked against that build.
#ifndef KERN_XELP_KERNEL_HPP
#define KERN_XELP_KERNEL_HPP
#include <stdint.h>
namespace XELP {
enum class Kernel : uint32_t { Unknown, BigSur20A2314a, BigSur20G1443, Tahoe25G241 };
struct KernelEntry { Kernel kernel; const char *version; };
constexpr KernelEntry KernelTable[] {
    {Kernel::BigSur20A2314a, "Darwin Kernel Version 20.0.0: Sat Jun 13 17:58:16 PDT 2020; root:xnu-7090.0.0.111.5~1/RELEASE_X86_64"},
    {Kernel::BigSur20G1443, "Darwin Kernel Version 20.6.0: Thu Jul  6 22:12:47 PDT 2023; root:xnu-7195.141.49.702.12~1/RELEASE_X86_64"},
    {Kernel::Tahoe25G241, "Darwin Kernel Version 25.6.0: Tue Aug 18 17:46:40 PDT 2026; root:xnu-12377.161.15.700.19~2/RELEASE_X86_64"},
};
inline bool sameVersion(const char *a, const char *b) {
    if (!a || !b) return false;
    while (*a && *a == *b) {a++;b++;}
    return *a == *b;
}
inline Kernel kernelIdentity(const char *version) {
    for (const auto &entry : KernelTable)
        if (sameVersion(version, entry.version)) return entry.kernel;
    return Kernel::Unknown;
}
// One predicate per private kernel assumption, so each admits only the
// builds that assumption was checked on.
// Display13 (ADL-N/ADL-P) profile admission. On 25G241 the donor kexts link
// unmodified into an auxiliary collection and every inherited vtable matches
// slot for slot. The private pmap routing (tglPrepareMMIOUC) stays 20G1443-only,
// so native acceleration remains fail-closed there.
inline bool display13Kernel(Kernel k) {return k == Kernel::BigSur20G1443 || k == Kernel::Tahoe25G241;}
// IGAccelFIFOChannel recovery caller contract. Its byte checks are on the
// donor kext, not the kernel.
inline bool recoveryKernel(Kernel k) {return k == Kernel::BigSur20G1443 || k == Kernel::Tahoe25G241;}
// IODTPlatformExpert::writeNVRAMProperty at vtable +0x9E8 (donor calls +0x9F8).
inline bool nvramSlotKernel(Kernel k) {return k == Kernel::BigSur20G1443 || k == Kernel::Tahoe25G241;}
// AppleIntelTGLCompat provider start: the donor framebuffer resolves these
// symbols on every kernel a donor path is admitted on, including 20A2314a.
inline bool providerKernel(Kernel k) {return k == Kernel::BigSur20A2314a || k == Kernel::BigSur20G1443 || k == Kernel::Tahoe25G241;}
}
#endif
