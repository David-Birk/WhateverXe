# Boot arguments

WhateverXe-specific arguments use the `wex` prefix. GPU selection and normal
backend features are automatic; family-selection flags are not required.
AppleIntelTGLCompat has no boot arguments.

## Normal configuration

| Argument | Purpose |
| --- | --- |
| `-wex-nopanel` | Omit the Apple driver's internal panel on the Tiger Lake path. Use on systems without an internal display. It has no effect on the separate Display13 topology. |
| `-wexdbg` | Enable WhateverXe debug logging in a debug build. |
| `-wexoff` | Disable WhateverXe. This alone is not a safe recovery configuration when its Apple drivers are still active. |
| `-wexbeta` | Allow the Lilu plugin's OS-version check to proceed on other versions. Exact kernel/driver checks still apply; this does not add OS support. |
| `-wex-tgl-off` | Disable the selected Tiger Lake path for troubleshooting. |

Alder Lake-N / Twin Lake automatically enables its framebuffer adaptation,
native accelerator and normal render-context profile when the GPU and OS meet
the backend's requirements. No per-feature enable arguments are needed.

The following names are shared with WhateverGreen or read by the Apple driver
and therefore keep their original spelling:

| Argument | Purpose |
| --- | --- |
| `-igfxdvmt` | Enable the inherited DVMT size-calculation correction for special GMS encodings. It does not increase firmware-allocated memory; ordinary GMS values need no correction. |
| `gfxrst=` | Select boot framebuffer handoff: 0 automatic (default), 1 reset, 2 copy, 3 clear, 4 disable. |
| `-allow3d` | Apple driver option used to enable its 3D path in the matching graphics setup. |
| `-disablegfxfirmware` | Apple driver option used to disable its graphics-firmware loading path in the matching graphics setup. |

These are individual controls, not a complete boot-argument preset. macOS and
Lilu settings depend on the root-patch installation. Do not copy another
machine's security or diagnostic settings as a universal configuration.
Additional bring-up and experimental switches in the source are not normal
configuration requirements.

## Migration

- Replace `-tglnopanel` with `-wex-nopanel` on Tiger Lake systems without an internal display.
- Replace `-wxeoff`, `-wxedbg`, `-wxebeta` with `-wexoff`, `-wexdbg`, `-wexbeta` if those controls are used.
- Replace `-igfxtgloff` with `-wex-tgl-off` if deliberately disabling the Tiger Lake path.
- Remove family selectors `-wex-adln`, `-wex-tgl` and their older spellings. Use automatic PCI selection or the explicit [device-id override](device-selection.md#advanced-override-device-id).
- Remove `-wex-display13`, `-wex-adln-native` and `-wex-adln-rcs-profile`; these normal features are automatic.

Old names and N97 aliases are not accepted. When moving from the old
argument-dependent provider, update WhateverXe, AppleIntelTGLCompat, the provider
in the System kernel collection, and the EFI arguments as a matching set.
WhateverXe 0.2.x uses provider 0.5.0; a WEX-only update within that pairing does
not itself require rebuilding the provider's kernel collection unless the
release instructions say otherwise.

The historical TGLCPUMemoryProbe and TGLLinkProbe kexts are not runtime
requirements. When migrating an installation that included them, remove their
bundles and exclude them from the rebuilt kernel collection. Retain the complete
predecessor configuration for recovery and check driver loading after migration.
