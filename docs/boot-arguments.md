# Boot arguments

All WhateverXe-owned arguments use `wex`. The only accepted spelling is the new name below; old names and N97 aliases are not accepted. Lilu-style plugin controls retain compact spellings; feature switches use hyphens. Hardware/donor admission guards remain independent of argument spelling.

## Normal configuration

- `-wexoff`, `-wexdbg`, `-wexbeta`: plugin disable, debug and unsupported-OS opt-in. Beta does not bypass exact kernel/donor checks. Disabling WEX while leaving its Apple donor active is not a safe recovery configuration.
- Family selection is automatic from PCI identity and the shared device table, with optional `device-id` override. `-wex-tgl-off` disables the selected TGL path.
- `-wex-nopanel`: omit the donor internal panel on the TGL path. The existing Display13 path has separate topology; this switch does not newly affect it.
- The admitted Alder-N/Twin Lake backend automatically enables Display13 framebuffer adaptation, the native accelerator, and its normal RCS context profile on the qualified kernel. `-wex-display13`, `-wex-adln-native` and `-wex-adln-rcs-profile` have been removed. AppleIntelTGLCompat consumes no boot arguments; WEX passes a validated profile and port policy through the binding API.

WEG-shared options remain `-igfxdvmt` and `gfxrst=`. Native Apple donor options `-allow3d` and `-disablegfxfirmware` also retain their names: renaming only our checks would not change what the donor reads. macOS/Lilu options are outside this namespace. DeviceProperties, IORegistry diagnostic names and the `adln-qtrace` executable name are unchanged.

## Migration

When migrating from the old argument-dependent provider, update WhateverXe, AppleIntelTGLCompat, the provider in the System KC, and EFI boot arguments together. Do not change only the arguments or only one kext. Once the argument-independent provider is installed, a WEX-only spelling change does not itself require a provider/KC update. Retain a matching predecessor pair/config/KC for recovery. The 0.2.0/0.5.0 pair and renamed arguments have been live-tested on T8. The 0.2.1 automatic-default change requires only WEX and EFI argument updates; provider0.5.0 and its KC remain compatible.

T8 normal arguments for WEX 0.2.1 (the inherited no-panel flag is retained, although inactive on Display13):

```text
debug=0x100 keepsyms=1 amfi=0x80 -wex-nopanel -allow3d -disablegfxfirmware -v -igfxdvmt
```

The family selectors `-wex-adln` and `-wex-tgl`, as well as their earlier spellings, have been removed. See [device selection](device-selection.md).

## Full rename table

Options beyond the normal configuration above are existing diagnostic/bring-up or disable switches. Renaming them does not make experimental combinations qualified or required for normal use. In particular, TC-cold bootstrap and AUX trials remain optional experiments. `-wex-adln-kernel-ctx` explicitly selects the existing context trial instead of the normal RCS profile; it is not a normal boot requirement. Other diagnostic combinations retain their existing guards and may be rejected.

| Accepted name | Removed spellings |
| --- | --- |
| `-wex-adln-config-stage` | `-adlnconfigstage`, `-n97configstage` |
| `-wex-adln-ctx-publish` | `-adlnctxpublish`, `-n97ctxpublish` |
| `-wex-adln-display-stage` | `-adlndisplaystage`, `-n97displaystage` |
| `-wex-adln-event-stage` | `-adlneventstage`, `-n97eventstage` |
| `-wex-adln-golden-raw` | `-adlngoldenraw`, `-n97goldenraw` |
| `-wex-adln-golden-source` | `-adlngoldensource`, `-n97goldensource` |
| `-wex-adln-gpu` | `-adlngpu`, `-n97gpu` |
| `-wex-adln-gt` | `-adlngt`, `-n97gt` |
| `-wex-adln-initial-no-pocs` | `-adlninitialnopocs`, `-n97initialnopocs` |
| `-wex-adln-kernel-ctx` | `-adlnkernelctx`, `-n97kernelctx` |
| `-wex-adln-kernel-early` | `-adlnkernelearly`, `-n97kernelearly` |
| `-wex-adln-kernel-gl` | `-adlnkernelgl`, `-n97kernelgl` |
| `-wex-adln-kernel-idle-gap` | `-adlnkernelidlegap`, `-n97kernelidlegap` |
| `-wex-adln-kernel-new-ctx` | `-adlnkernelnewctx`, `-n97kernelnewctx` |
| `-wex-adln-kernel-render-wake` | `-adlnkernelrenderwake`, `-n97kernelrenderwake` |
| `-wex-adln-kernel-reuse` | `-adlnkernelreuse`, `-n97kernelreuse` |
| `-wex-adln-kernel-wake` | `-adlnkernelwake`, `-n97kernelwake` |
| `-wex-adln-load-only` | `-adlnloadonly`, `-n97loadonly` |
| `-wex-adln-memory-stage` | `-adlnmemorystage`, `-n97memorystage` |
| `-wex-adln-parent-entry` | `-adlnparententry`, `-n97parententry` |
| `-wex-adln-parent-only` | `-adlnparentonly`, `-n97parentonly` |
| `-wex-adln-probe` | `-adlnprobe`, `-n97probe` |
| `-wex-adln-qtrace` | `-adlnqtrace`, `-n97qtrace` |
| `-wex-adln-rcs-no-pocs` | `-adlnrcsnopocs`, `-n97rcsnopocs` |
| `-wex-adln-scanout` | `-adlnscanout`, `-n97scanout` |
| `-wex-adln-task-stage` | `-adlntaskstage`, `-n97taskstage` |
| `-wex-display13-trace` | `-display13trace`, `-n97displaytrace` |
| `-wex-tgl-off` | `-igfxtgloff` |
| `-wex-nopanel` | `-tglnopanel` |
| `-wex-tgl-prog-off` | `-tglprogoff` |
| `-wex-no-host-rps` | `-tglnohostrps` |
| `-wex-tgl-hold-render-wake` | `-tglholdrenderwake` |
| `-wex-tc-aux` | `-tgltc-aux` |
| `-wex-tc-bootstrap` | `-tgltc-bootstrap` |
| `-wexoff` | `-wxeoff` |
| `-wexdbg` | `-wxedbg` |
| `-wexbeta` | `-wxebeta` |

## Provider selection

AppleIntelTGLCompat has no user-facing options. Its exact-kernel gate remains at
startup, and backend selection happens on the actual owner or registered binding:

- Register and DSB methods require a TGL backend selected by the shared PCI/DeviceProperties resolver.
- WEX chooses admission and wiring, then passes its physical/effective profile and port policy to
  `xelpConfigureDisplay` (ABI3). The provider independently resolves PCI/DeviceProperties and validates the ABI,
  controller/manager/map and platform before pinning the association.
- Without a binding, port probing retains the old TGL fallback only for the
  manager previously established by a controller operation using a validated TGL profile.
  Unbound owners without such an association return mode0; no generation is guessed.
- The legacy ABI1 entry resolves and validates the Alder-N profile and feeds the same
  binding implementation, with no boot-argument prerequisite.

This removes duplicated boot policy; it does not make the provider a standalone
driver or automatically admit new hardware, ports or transports.
