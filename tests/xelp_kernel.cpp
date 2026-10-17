#include "kern_xelp_kernel.hpp"
#include <cassert>
#include <cstring>
#include <string>
using namespace XELP;
int main() {
 for(const auto &entry:KernelTable) {
  assert(kernelIdentity(entry.version)==entry.kernel);
  // Prefixes, extensions and single-character changes are other kernels.
  std::string v=entry.version;
  assert(kernelIdentity(v.substr(0,v.size()-1).c_str())==Kernel::Unknown);
  assert(kernelIdentity((v+" ").c_str())==Kernel::Unknown);
  v[v.size()-2]^=1;assert(kernelIdentity(v.c_str())==Kernel::Unknown);
 }
 assert(kernelIdentity(nullptr)==Kernel::Unknown && kernelIdentity("")==Kernel::Unknown);
 // 20A2314a is the donor's own kernel; no Display13 or ported fix admits it.
 assert(!display13Kernel(Kernel::BigSur20A2314a) && !recoveryKernel(Kernel::BigSur20A2314a));
 assert(!nvramSlotKernel(Kernel::BigSur20A2314a));
 // The provider also serves the donor's own kernel.
 for(auto k:{Kernel::BigSur20A2314a,Kernel::BigSur20G1443,Kernel::Tahoe25G241})assert(providerKernel(k));
 assert(!providerKernel(Kernel::Unknown));
 for(auto k:{Kernel::BigSur20G1443,Kernel::Tahoe25G241})
  assert(display13Kernel(k) && recoveryKernel(k) && nvramSlotKernel(k));
 assert(!display13Kernel(Kernel::Unknown) && !recoveryKernel(Kernel::Unknown) && !nvramSlotKernel(Kernel::Unknown));
}
