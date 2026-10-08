# WhateverXe

Xe-LP compatibility plugin for Lilu, extracted from the Xe development fork of WhateverGreen at 0570802.
Bundle: `WhateverXe.kext`, identifier: `org.b00t0x.WhateverXe`.
Provider: `AppleIntelTGLCompat.kext`, identifier: `org.b00t0x.AppleIntelTGLCompat`.

The provider supplies missing methods for the exact Apple TGL donor; it is not a GPU driver by itself. Existing Apple kernel/userland donors remain required. No new GPU or transport is admitted by this extraction.

Upstream WhateverGreen copyright and BSD license are retained. See LICENSE.txt and docs/extraction-map.json. Lilu and MacKernelSDK are separate build dependencies, not vendored source.

Legacy N97-prefixed IORegistry keys and boot argument aliases remain for existing collectors/configurations. Internal names distinguish Display13, AlderLakeN, XELP and TGL donor contracts. This does not qualify every GPU with those display generations.

## Build and test

Provide `MacKernelSDK/` and `Lilu.kext/Contents/Resources/{Headers,Library}` locally, or pass `--sdk` and `--lilu` explicitly. No WEG source or binary is needed.

```sh
python3 tools/build.py
python3 tools/test.py --donor /path/to/AppleIntelTGLGraphicsFramebuffer
python3 tools/stage_root.py --baseline /path/to/qualified-root --build build --output /path/to/new-root
```

The optional test donor must be the exact c004fc1a framebuffer. Build outputs include both kexts and a source/header manifest. `stage_root.py` validates the predecessor manifest, preserves Apple executables, and updates the framebuffer provider dependency. Rebuild the target System KC after migrating the provider identity.

The historical `TGLCPUMemoryProbe` and `TGLLinkProbe` kexts are diagnostic tools, not runtime dependencies of WhateverXe. The normal root payload excludes both. When migrating an older installation, remove separately installed probe bundles and exclude their identifiers from the rebuilt KC; removing files alone does not remove cached kexts. Verify normal driver loading after removal, since TGLLinkProbe previously triggered dependency loading. MI646 removal results and recovery assets are recorded in the enclosing project's Xe records; other machines require their own validation.

Canonical switches include `-igfxadln`, `-adlnnative`, `-display13`, `-display13trace`, `-wxeoff`, `-wxedbg`, and `-wxebeta`. Existing `-igfxn97`/`-n97*` switches remain aliases; old WEG global switches are not WhateverXe switches. Physical PCI/revision and exact kernel/donor checks remain authoritative. DVMT keeps its established `-igfxdvmt`/device-property opt-in.

Disable the Xe development WEG when enabling WhateverXe. This revision has no qualified WEG co-installation mode, including mixed-GPU configurations. Its boot framebuffer hook is global. Do not enable both and rely on load order.

## Extraction validation

Initial CPU qualification: 50 suites passed, including Display13 resource/power/IRQ/WM/cursor, Alder-N GT, shared profile/provider binding, DVMT calculations, TGL wiring/training leases and exact donor TC replay. This does not qualify new hardware or a physical third output. Machine-specific runtime results are maintained in the enclosing project's Xe records.

Historical scripts requiring private evidence or old WEG git revisions remain in the predecessor repository. The DKL regression counterexample is retained as a small licensed fixture. This repository does not require access to that old git history for its normal test command.
