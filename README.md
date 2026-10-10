# WhateverXe

[日本語](README_ja.md)

WhateverXe is a Lilu plugin that adapts Apple Intel graphics drivers for selected
Intel Xe-LP integrated GPUs. It is derived from WhateverGreen.

This project is experimental. It requires the Xe root patch supplied by the
[b00t0x OpenCore Legacy Patcher fork](https://github.com/b00t0x/OpenCore-Legacy-Patcher);
installing WhateverXe alone is not sufficient.

## Compatibility

The current target is **macOS Big Sur build 20G1443 (x86_64)** with the matching
Apple TGL kernel and userland drivers. Other OS builds and arbitrary driver
versions are not supported by the current implementation.

| GPU family | Device IDs with an implemented backend |
| --- | --- |
| Tiger Lake | `9A40`, `9A49`, `9A78` |
| Alder Lake-N / Twin Lake | `46D0`–`46D4` (PCI revision 0) |

An implemented backend is not a guarantee for every system using that ID.
The device table also contains identification-only entries for other Intel GPUs;
those entries do not enable graphics support. See [device selection](docs/device-selection.md).

The tested feature set includes dual external 1920×1080 at 60 Hz output,
Metal/OpenGL, hardware HEVC, and display sleep/wake on Tiger Lake and Alder Lake-N.
Hardware H.264 encode/decode has also been tested on Tiger Lake. Validation is
limited to specific systems; USB-C/Thunderbolt display paths, three simultaneous
displays, internal panels and system sleep are not established as supported
configurations. Higher resolutions and other connector layouts require separate
validation.

## Required components

| Component | Role |
| --- | --- |
| Lilu | Runtime required by WhateverXe |
| `WhateverXe.kext` (`org.b00t0x.WhateverXe`) | Graphics compatibility plugin, loaded through OpenCore after Lilu |
| [b00t0x OpenCore Legacy Patcher](https://github.com/b00t0x/OpenCore-Legacy-Patcher) | Apply its Xe root patch to install the matching graphics stack |
| [AMFIPass v1.4.1 Mod](https://github.com/b00t0x/OpenCore-Legacy-Patcher/blob/mod/payloads/Kexts/Acidanthera/AMFIPass-v1.4.1-Mod-RELEASE.zip) | Required for the modified root-patched graphics stack; install through OpenCore |

The OCLP fork's Xe root patch is implemented but has not yet been released.
Use the Xe-capable version of this fork. OCLP installs the required root-patch
components and updates the kernel collection.

Extract `AMFIPass.kext` from the linked **Mod** archive, place it in
`EFI/OC/Kexts`, and enable it in OpenCore's `Kernel > Add`. Replace an existing
AMFIPass with this Mod build; do not load both copies.

Use WhateverXe with the matching OCLP root patch, and retain a working EFI/root
configuration for recovery. Disabling WhateverXe alone while the root-patched
drivers remain active can prevent startup.

WhateverGreen and WhateverXe must not be enabled together. Co-installation,
including mixed-GPU configurations, is not supported.

## Configuration

GPU selection is automatic from the PCI device ID. A `device-id` override is an
advanced backend selector, not a way to supply missing hardware capabilities.

Connector routing is read from firmware VBT where the current backend can
interpret it. Supported fixed HDMI and fixed DisplayPort layouts do not normally
need manual wiring properties. Automatic detection does not add USB-C, dock or
other unsupported transports. On Tiger Lake systems without an internal display,
use `-wex-nopanel`.

- [Boot arguments and migration](docs/boot-arguments.md)
- [Device selection and DeviceProperties](docs/device-selection.md)

## Build and test

Use macOS with Xcode command-line tools, Python 3.9 or newer, MacKernelSDK and
Lilu's development headers/library. Place `MacKernelSDK/` and `Lilu.kext/` in the
repository root, or pass their paths with `--sdk` and `--lilu`.

```sh
python3 tools/build.py --release
python3 tools/test.py
```

The build produces kexts and a source manifest in `build/`. CPU tests do not
establish compatibility with a physical GPU. Optional donor replay tests accept
`--donor /path/to/AppleIntelTGLGraphicsFramebuffer` and
`--accelerator /path/to/AppleIntelTGLGraphics`; they require the exact driver
binaries accepted by the respective tests. WhateverGreen source is not a build
dependency.

## License and credits

WhateverXe retains WhateverGreen's copyright notices and BSD-3-Clause license.
See [LICENSE.txt](LICENSE.txt). The historical [extraction map](docs/extraction-map.json)
records its source origins. Lilu and MacKernelSDK are separate dependencies.
