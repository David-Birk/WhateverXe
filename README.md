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

## Firmware connector defaults

On admitted Tiger Lake and Alder Lake-N hardware, WhateverXe reads the ACPI
OpRegion VBT before selecting framebuffer hooks. OpRegion 2.0 absolute and 2.x
relative external VBT addresses, plus the embedded VBT, are supported. The bounded
parser currently accepts BDB versions 216–256. Missing, malformed or unsupported
firmware never supplies guessed wiring; existing explicit properties remain usable.

An unambiguous fixed TMDS entry for physical Combo A/B can supply the HDMI DDI
and DDC pin without a wiring property. TGL uses `tgl-hdmi-wiring`; Alder-N uses
`xelp-hdmi-wiring` with the existing TGL alias. Present properties override firmware,
and malformed overrides are rejected. Firmware facts do not expand the qualified
transport mask. DP++, LSPCON, USB-C/TBT, lane reversal, dock routing and inverted
HPD entries do not become automatic fixed HDMI. The VBT DVO label alone does not
select DP versus HDMI: some firmware names HDMI ports DP-A/DP-B.

`WhateverXeFirmwarePorts` on IGPU contains twelve little-endian words: schema(1),
parser status (1=valid), source (1=embedded, 2=external), BDB version, physical DDI
mask, unmapped-entry count, non-fixed-TMDS mask, automatic-HDMI validity, HDMI mask,
and DDC pins A/B/C. `WhateverXeFirmwarePortRecords` retains nine records of six
words: device type, original DVO, DDC, AUX, VBT byte23 flags, and byte33 capabilities.
USB-C/TBT capability is not the current link mode.

Without `tgl-tc-wiring`, TGL external DP entries with matching DDI/AUX routing
can become probe candidates. The automatic backend currently admits TC1/TC2:
the native mode transaction now maps qualified modular FIA coordinates for
TC1–TC4, but this does not extend physical-port admission. Reversed lanes, inverted HPD, dock/LSPCON,
shared or remapped AUX and internal panels are excluded. Explicit properties,
including invalid ones, override automatic discovery.

The provider still checks live HPD/FIA and only a successful mode4 probe registers
the port for existing fixed-DP PHY/AUX fixes. Registration persists through
disconnect to keep power gets/puts symmetric; it is neither connection state nor
a power reference. Automatic ports reject mode2/3 (TBT/DP-alt). Diagnostics on
the registry root are `WhateverXeTCDPCandidates` and `WhateverXeTCDPRegistered`
(physical-DDI bitmasks). No CPU-to-board lookup is involved. `-tglnopanel` remains
available. ROM scanning, automatic panel setup, DP++ protocol fallback and new
Type-C transports are not implemented by this change.

## Native FIA adapter and diagnostics

The exact-donor mode0/4 transaction uses the TGL port coordinate model for its
FIA ready/ownership accesses. A successful provider mode4 probe, the existing mode
mutex, the current thread/port and five matched native call sites are required.
TC1/TC2 retain the original MMIO/AUX sequence. TC3/TC4 bank/bit mapping and sibling
bit preservation are CPU-tested, not live-qualified. TC5/TC6, unavailable-register
handling, TC-cold boot handoff and repeated mode4 AUX acquisition remain separate
work; this adapter does not provide a full mode lifetime implementation.

`WhateverXeNativeFiaReady` reports routing qualification. `WhateverXeNativeFiaAccess`
is six u32 values: schema1, physical DDI, FIA instance, bit mask, cumulative reads,
and writes. Counts record adapter accesses, not hardware acknowledgement.
Activation/path/engine observers now record software state without implicit
fixed-port MMIO. DPCD records are 80 bytes: ns/thread u64 followed by sixteen u32;
word12 is physical DDI, word13 is schema1. The new `WhateverXeDPCD*` properties
replace `TGLDPCD4*`. Optimal-lane records include DDI at qword11.
The obsolete `-tglcrc`, `-tglholdfirstd3`, and `-tglcursorpte` experiments were
removed; these switches no longer perform register/AUX/PTE writes.


`WhateverXeTCControl3` through `WhateverXeTCControl8` are live, memory-only
snapshots: four u32 words (schema1, physical DDI, all-event count, control count),
then 64 ring slots of the existing 128-byte MEM1 AUX record. Timestamp and sequence
identify valid records; event7 is provider entry/return, event8 mode-lock wait/
acquisition, event9 parent-PG get return/put entry. Event1 and events5/6 retain mode
and AUX boundaries. These snapshots do not require EDID-time publication and
contain no register reads. Reading them does not establish hardware liveness.

The provider probe currently assumes powered FIA access; registration does not
prove TC-cold protection. The optional whole-boot `-tgltc-bootstrap` experiment
is not a production lifetime fix. It did not prevent the observed MI646 reconnect
hang after successful mode0-to-4 and subsequent AUX calls. Runtime hotplug is
not yet qualified; do not infer it from normal startup or display sleep/wake.


Scanout and HDCP protocol observers are now memory-only as well. Their callbacks
include pre-allocation/post-disable boundaries with no display/GT power lease.
`TGLScan*` retains its 256-qword layout with MEM1 at qword255; old MMIO/GGTT value
slots are unsampled. `TGLHDCP1*` uses qword11=MEM1 and qword10=0 (unsampled).
TC control event10 brackets port training, event11 brackets LightUpDP. Native
training, mode, power, scanout and HDCP operations remain unchanged.
