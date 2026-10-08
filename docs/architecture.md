# Responsibilities after extraction

- `kern_host`: Lilu callback wiring, admitted physical GPU attributes and WEG-derived boot framebuffer handoff. No graphics ID spoofing, AGDP, AMD/NVIDIA or legacy Intel patch module tree.
- `kern_xe`: retained Xe hook bodies and donor orchestration. The large body remains together where static state is shared; moving it is separate from changing lifetime/ownership.
- `kern_dvmt`: WEG-derived, independently testable DVMT calculation repair. No original PatchSubmodule dependency. Normal GMS values do not need instruction repair.
- `kern_display13_*`: Display13 framebuffer, clock, watermark, DDB, power, cursor and transport contracts. Admission remains the measured Alder-N/Twin Lake set.
- `kern_alderlake_n*`: small-GT physical admission and qualified GT paths. Not an admission switch for arbitrary Display13 GPUs.
- `kern_xelp_*`: shared profile/binding, physical memory layout, geometry and bounded queue observations.
- `kern_tgl_*`: TGL donor ABI, existing Display12 transport and shared donor helpers.
- `AppleIntelTGLCompat`: missing Apple methods, native owner validation and shared port probe. It has no reverse dependency on WhateverXe; the plugin registers the typed ABI2 binding.

External N97-prefixed diagnostic keys are retained for existing collectors. They do not name internal types. Legacy boot spellings are accepted by the alias layer. Apple C++ exports and existing hardware admission remain unchanged.

The WEG upstream source baseline is 0762cecc2a70054cd4dc3cf4d08979aca6acd9bb. Extraction uses the qualified Xe fork at 0570802. Copyright notices and the upstream license are retained; provider source originates in oclp tools/xe/TGLCompat.cpp at the 0.3.0 shared-binding milestone.

MI646 DVMT96MB is the first live migration condition. Extracted DVMT opt-in behaviour is preserved, but special GMS live qualification and T8 Display13 runtime qualification remain separate from initial CPU tests.
