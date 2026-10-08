#ifndef KERN_ALDERLAKE_N_FORCEWAKE_IRQ_HPP
#define KERN_ALDERLAKE_N_FORCEWAKE_IRQ_HPP
namespace AlderLakeNForceWakeIRQ {
// The exact native regular method holds a non-IRQ-safe spinlock across its
// refcount and multithreaded register/ACK transaction. Prevent same-CPU GT IRQ
// submit from recursively acquiring that lock. Preserve prior IF, including
// already-disabled interrupt callers, and every native count/client/ACK.
template<class Set,class Call> void protect(Set set,Call call) {
    const auto prior=set(false);
    call();
    set(prior);
}
}
#endif
