# WhateverXe

[English](README.md)

WhateverXeは、一部のIntel Xe-LP内蔵GPU向けにAppleのグラフィックスドライバを適応させるLiluプラグインです。WhateverGreenを基に開発しています。

本プロジェクトは実験段階です。[b00t0x版OpenCore Legacy Patcher](https://github.com/b00t0x/OpenCore-Legacy-Patcher)が提供するXe root patchが必要であり、WhateverXeだけでは動作しません。

## 対応範囲

現在の対象は、対応するApple TGLのカーネル／ユーザーランドドライバを組み合わせた **macOS Big Sur build 20G1443（x86_64）** です。現在の実装は、他のOSビルドや任意のドライババージョンには対応していません。

| GPUファミリー | バックエンド実装があるdevice ID |
| --- | --- |
| Tiger Lake | `9A40`、`9A49`、`9A78` |
| Alder Lake-N / Twin Lake | `46D0`–`46D4`（PCI revision 0） |

バックエンドが実装されていても、そのIDを使うすべての機種での動作を保証するものではありません。デバイステーブルには識別用の情報だけを持つ他のIntel GPUも含まれますが、それらのグラフィックス機能は有効になりません。[デバイス選択](docs/device-selection.md)を参照してください。

Tiger LakeとAlder Lake-Nの特定の実機で、外部2画面の1920×1080・60Hz出力、Metal/OpenGL、ハードウェアHEVC、画面の消灯／復帰を確認しています。Tiger LakeではハードウェアH.264のエンコード／デコードも確認しています。USB-C/Thunderbolt経由の画面出力、3画面同時出力、内蔵パネル、システムスリープは、対応構成として確立していません。より高い解像度や他のコネクタ構成は別途検証が必要です。

## 必須構成

| コンポーネント | 役割 |
| --- | --- |
| Lilu | WhateverXeの動作に必要なランタイム |
| `WhateverXe.kext`（`org.b00t0x.WhateverXe`） | グラフィックス互換プラグイン。OpenCoreでLiluより後に読み込む |
| [b00t0x版OpenCore Legacy Patcher](https://github.com/b00t0x/OpenCore-Legacy-Patcher) | Xe root patchを適用し、対応するグラフィックス構成を導入する |
| [AMFIPass v1.4.1 Mod](https://github.com/b00t0x/OpenCore-Legacy-Patcher/blob/mod/payloads/Kexts/Acidanthera/AMFIPass-v1.4.1-Mod-RELEASE.zip) | 改変されたroot patchのグラフィックス構成に必要。OpenCoreで導入する |

このOCLP forkにはXe root patchが実装済みですが、まだリリース前です。このforkのXe対応版を使用してください。root patchに必要な構成要素の導入とkernel collectionの更新はOCLPが行います。

リンク先の **Mod版** アーカイブから`AMFIPass.kext`を展開し、`EFI/OC/Kexts`へ配置して、OpenCoreの`Kernel > Add`で有効にしてください。既存のAMFIPassがある場合はこのMod版へ置き換え、両方を同時に読み込まないでください。

WhateverXeと対応するOCLPのroot patchを組み合わせ、復旧用に動作するEFI／root構成を保持してください。root patchのドライバを有効にしたままWhateverXeだけを無効にすると、起動できなくなる場合があります。

WhateverGreenとWhateverXeは同時に有効にしないでください。複数GPUの構成を含め、併用には対応していません。

## 設定

GPUはPCI device IDから自動選択します。`device-id`の上書きは高度な設定として既存バックエンドを選択するもので、不足するハードウェア機能を補うものではありません。

現在のバックエンドが解釈できる範囲で、ファームウェアのVBTからコネクタの配線を読み取ります。対応する固定HDMI／固定DisplayPort構成では、通常は配線propertyの手動指定は不要です。自動検出によってUSB-C、ドックなどの未対応経路が使えるようになるわけではありません。内蔵画面のないTiger Lake機では`-wex-nopanel`を指定してください。

- [ブート引数と移行](docs/boot-arguments.md)
- [デバイス選択とDeviceProperties](docs/device-selection.md)

## ビルドとテスト

macOS、Xcode command-line tools、Python 3.9以降、MacKernelSDK、Liluの開発用ヘッダ／ライブラリを使用します。`MacKernelSDK/`と`Lilu.kext/`をリポジトリ直下に配置するか、`--sdk`と`--lilu`でパスを指定してください。

```sh
python3 tools/build.py --release
python3 tools/test.py
```

ビルド結果のkextとソースのmanifestは`build/`に出力されます。CPUテストだけで実GPUの互換性が確認できるわけではありません。任意のdonor再生試験には`--donor /path/to/AppleIntelTGLGraphicsFramebuffer`と`--accelerator /path/to/AppleIntelTGLGraphics`を指定できますが、各試験が受け付ける正確なドライババイナリが必要です。ビルドにWhateverGreenのソースは不要です。

## ライセンスとクレジット

WhateverXeはWhateverGreenの著作権表示とBSD-3-Clauseライセンスを継承しています。[LICENSE.txt](LICENSE.txt)を参照してください。[抽出時の対応表](docs/extraction-map.json)にソースの由来を記録しています。LiluとMacKernelSDKは別の依存コンポーネントです。
