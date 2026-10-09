# Responsibilities after extraction

- `kern_host`: Lilu callback wiring, physical/effective GPU selection and model attributes and WEG-derived boot framebuffer handoff. No global PCI read spoof hooks, AGDP, AMD/NVIDIA or legacy Intel patch module tree.
- `kern_xe`: retained Xe hook bodies and donor orchestration. The large body remains together where static state is shared; moving it is separate from changing lifetime/ownership.
- `kern_dvmt`: WEG-derived, independently testable DVMT calculation repair. No original PatchSubmodule dependency. Normal GMS values do not need instruction repair.
- `kern_display13_*`: Display13 framebuffer, clock, watermark, DDB, power, cursor and transport contracts. The implemented backend remains the Alder-N/Twin Lake small-GT path; an explicit device-id override is a backend trial, not new hardware qualification.
- `kern_alderlake_n*`: small-GT profile checks and qualified GT paths. Not an admission switch for arbitrary Display13 GPUs.
- `kern_xelp_*`: shared device table, physical/effective profile/binding, physical memory layout, geometry and bounded queue observations.
- `kern_tgl_*`: TGL donor ABI, existing Display12 transport and shared donor helpers.
- `AppleIntelTGLCompat`: missing Apple methods, native owner validation and shared port probe. It has no reverse dependency on WhateverXe; the plugin registers the typed ABI3 binding.

External N97-prefixed diagnostic keys are retained for existing collectors. They do not name internal types. Legacy boot spellings are removed. PCI/DeviceProperties select the family automatically; Apple C++ exports remain unchanged. See [the device selection contract](device-selection.md) for explicit fake-ID semantics and qualification limits.

The WEG upstream source baseline is 0762cecc2a70054cd4dc3cf4d08979aca6acd9bb. Extraction uses the qualified Xe fork at 0570802. Copyright notices and the upstream license are retained; provider source originates in oclp tools/xe/TGLCompat.cpp at the 0.3.0 shared-binding milestone.

MI646 DVMT96MB is the first live migration condition. Extracted DVMT opt-in behaviour is preserved, but special GMS live qualification and T8 Display13 runtime qualification remain separate from initial CPU tests.
