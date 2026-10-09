# PCI profiles and DeviceProperties

WhateverXe 0.2.0 and AppleIntelTGLCompat 0.5.0 use one device table and resolver.
The provider has no boot arguments. This revision is CPU/build tested; its new
selection/override behavior has not yet been qualified on live hardware.

## Selection

1. Register donor notifications during plugin initialization. This does not load
   or activate a GPU driver. Lilu runs the kernel callback before queued donor
   callbacks, including donors already present in the kernel collection.
2. Wait for the Intel IOPCIDevice to be published. Read its physical vendor/device
   ID and revision from config space. CPUID is not used to select the GPU family.
3. Use the `device-id` DeviceProperty, when present, as the table selector. It must
   be exactly four bytes of little-endian Data containing a 16-bit device ID.
   With no property, use the physical device ID. IOPCIFamily's normal property
   with the same ID also selects the physical profile.
4. Select the implemented backend, then apply the existing exact-kernel/donor,
   BAR/map, fuse/resource, memory and transport checks for that backend.

For example, `device-id` Data `D2460000` selects the 0x46D2 Alder-N profile,
including for an unknown physical Intel ID. It does not rewrite PCI config reads.
Wrong type/length, zero/FFFF, an unknown target ID, a target without an implemented
backend or a non-Intel physical vendor do not fall back to another profile.
Physical revision is retained; selecting Alder-N still requires revision0.
Neither names nor fake IDs fabricate EU counts, memory sizes or board wiring.

The old family selectors `-wex-adln` and `-wex-tgl` are removed. The existing
feature/diagnostic controls (`-wex-display13`, `-wex-adln-native`,
`-wex-adln-rcs-profile`, etc.) remain opt-ins; family detection does not enable
these features automatically. `-wex-tgl-off` remains a disable switch.
An old ICL or other unsupported `device-id` spoof is now an explicit unsupported
selector: remove it to use the physical profile or replace it with an intended
implemented Xe ID. It is never silently ignored.

## Table and names

`kern_xelp_devices.hpp` has 36 unique IDs. Family/Display metadata includes TGL,
RKL, ADL-S/P/N, RPL-S/P/U and Twin Lake (which shares the Alder-N backend family).
Only the previous eight IDs have an automatic backend: TGL 9A40/9A49/9A78 and
Alder-N/Twin Lake 46D0–46D4. Other entries provide identification and names, not
new hardware support. Explicit overrides may select an existing backend; a
successful lookup is not a hardware compatibility result.

The user's `model` property is preserved. Otherwise, use the physical ID's known
name, falling back to the selected ID's name when the physical ID is unknown.
Names are suggested labels, not proof of marketing branding (which can depend on
SKU or system configuration), and do not influence backend selection.

Input provenance: `device-id.csv`, **Provided by b00t0x**. It contains 61 rows,
including repeated IDs for multiple SKUs/EU counts. The supplied names are reference
values and have not been independently verified against Intel product pages.
All 36 IDs and their backend-family grouping were checked against upstream Linux
v6.12 `include/drm/i915_pciids.h`; Display12/13 groupings were checked against
`drivers/gpu/drm/i915/display/intel_display_device.c`. This check does not verify
per-SKU names or EU counts. No EU values from the CSV are encoded in the table.
The raw CSV and local source hashes are retained in the enclosing project's
ignored materials and task record. No downloaded Apple donor is included here.

## Shared contract and donor handling

A profile keeps **physical identity/revision** and **effective identity** separate.
WEX binds the selected profile to the actual GPU object. Later guards check the
same owner and resolution, rather than rejecting all non-table physical IDs.
Changing `device-id` while running is not a supported way to change profiles;
existing associations are immutable until reboot and mismatches are rejected.

The ABI3 `ProbeConfig` passes both identities and the selected Display/GT/donor
profile. AppleIntelTGLCompat independently resolves the controller's physical
PCI/DeviceProperties and rejects mismatched or malformed registration. TGL
register/DSB operations, including pre-binding calls, use that same resolver.
Unbound port probing still requires a TGL manager established by those operations.
The legacy ABI1 entry adapts to the same resolver and binding.

The native accelerator probe reads PCI config space, not the profile selector.
Its guarded SKU comparison therefore accepts the **physical** ID, while profile
selection controls the backend. The existing signed-ID split is also normalized
for explicitly overridden profiles so a positive/other unknown ID cannot be sent
to the donor simulator branch merely because of its numeric value. Resource and
exact-byte guards remain in place; unknown hardware is not live-qualified by this.

`WhateverXeDeviceSelection` on IGPU is eight little-endian u32 words:
schema1, physical vendor/device dword, physical revision, effective vendor/device
dword, Family enum, Display enum, GT enum, backend admission. The last word is a
selection result, not proof that the donor started or rendering succeeded.

## Upgrade

ABI3 changes the profile layout. Upgrade WEX, the provider in System/root and KC,
and the boot configuration as a matching set. WEX 0.1.x/provider0.4.x ABI2 must not
be mixed with this pair. Old ABI2 registrations are rejected before reading their
old profile layout. Apple executable files are unchanged; `stage_root.py` can
update either the old TGLCompat or already renamed AppleIntelTGLCompat bundle and
refresh its framebuffer dependency version. This is an offline candidate only.
