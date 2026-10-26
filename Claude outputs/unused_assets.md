# 未使用アセット一覧

## 対応結果（2026-10-26）

以下の方針で整理を実施済み。

### 判断事項

| 対象 | 判断 | 理由・備考 |
|---|---|---|
| `assets/data/*_init.json`（3ファイル） | 削除 | — |
| `assets/data/ReadMe.txt` | 削除 | — |
| `assets/shader/hlsl/`（14ファイル） | **残す** | ポートフォリオ閲覧者がシェーダーのソースを見る可能性があるため |
| `assets/picture/UI1/Divider Fade/divider-fade-002.png` | **移動** | `assets/picture/divider-fade-002.png` へ移動（`git mv`） |
| 上記以外の A・B・C 区分のファイル | 削除 | — |

### 実施内容

- `divider-fade-002.png` を `assets/picture/` 直下へ移動し、`assets/data/objectData.json` 内の参照2箇所（332行目・1353行目）を書き換えた
  - 変更前：`"assets/picture\\UI1/Divider Fade/divider-fade-002.png"`
  - 変更後：`"assets/picture/divider-fade-002.png"`
- 未使用ファイル **663 ファイル** を `git rm` で削除（空になったフォルダも消えた）
  - 削除後の `assets/` の容量は約 84 MB
  - フォルダごと消えたもの：`assets/Models/`, `assets/mapdata/`, `assets/picture/UI1/`, `assets/picture/UI2/PNG/Double/`, `assets/picture/UI2/Vector/`, `assets/picture/UI2/Spritesheet/`
- 削除後に再スキャンした結果、未使用は `assets/shader/hlsl/`（意図的に残したもの）だけで、参照先が見つからないファイルは増えていない

### 未対応（必要なら別途対応）

- 「補足」に書いたビルド生成物（`x64/` など）、`imgui.ini`、`DirectX12.vcxproj.user` は今回は削除していない。配布用zipを作るときに含めないようにする

### 追加対応：呼び出されていない関数の削除

存在しない `spriteData.json` / `textData.json` を読み書きしていた、呼び出し元のない関数を削除した。

- `SpriteComponent::loadFileAndCreate`（`SpriteComponent.h` / `.cpp`）
- `TextComponent::loadFileAndCreate`（`TextComponent.h` / `.cpp`）
- 上記の削除で使われなくなった `#include <fstream>`（`SpriteComponent.cpp` の重複2箇所、`TextComponent.cpp` の1箇所）

Release / Debug（x64）とも、ビルドが通ることを確認済み。

---

以下は整理前の調査結果。

配布前の不要ファイル整理用。`DirectX12/assets/` 以下の全ファイルについて、以下のいずれからもパスが参照されていないものを「未使用」と判定した。

- ソースコード（`scripts/game`, `scripts/engine` の `.cpp` / `.h`）※コメントアウトされた行の参照は除外
- データJSON（`assets/data/*.json`：meshData / soundData / objectData など）
- モデルJSON（`assets/3DModels/**/*.json` の `texturePath`）

パスはすべて `DirectX12/` からの相対パス。

**未使用ファイル合計：677 ファイル / 約 50.65 MB**

## サマリー

| 区分 | 対象 | ファイル数 | サイズ |
|---|---|---:|---:|
| A. フォルダごと削除可 | `assets/Models/` | 116 | 10.62 MB |
| A. フォルダごと削除可 | `assets/picture/UI1/` | 139 | 24.4 KB |
| A. フォルダごと削除可 | `assets/picture/UI2/PNG/Double/` | 128 | 126.2 KB |
| A. フォルダごと削除可 | `assets/picture/UI2/Vector/` | 128 | 224.3 KB |
| A. フォルダごと削除可 | `assets/picture/UI2/Spritesheet/` | 4 | 160.0 KB |
| A. フォルダごと削除可 | `assets/mapdata/` | 1 | 0.3 KB |
| B. 個別ファイル削除 | `assets/sounds/BGMs/` | 9 | 23.96 MB |
| B. 個別ファイル削除 | `assets/sounds/SEs/` | 6 | 13.47 MB |
| B. 個別ファイル削除 | `assets/picture/` | 4 | 1.91 MB |
| B. 個別ファイル削除 | `assets/picture/UI2/PNG/Default/` | 116 | 68.3 KB |
| B. 個別ファイル削除 | `assets/picture/UI2/` | 5 | 78.4 KB |
| B. 個別ファイル削除 | `assets/3DModels/rockObj/` | 3 | 1.7 KB |
| C. 要確認 | `assets/data/` | 4 | 7.3 KB |
| C. 要確認 | `assets/shader/hlsl/` | 14 | 14.8 KB |

## A. フォルダごと削除してよいもの

フォルダ内に使用中のファイルがない（または例外が1つだけ）もの。

### `assets/Models/` — 116 ファイル / 10.62 MB

Kenney のダンジョンキット（FBX/GLB/OBJ）。コードからもmeshDataからも参照なし。3Dモデルは `assets/3DModels` を使用している。

<details><summary>ファイル一覧</summary>

- `assets/Models/FBX format/colormap.png`
- `assets/Models/FBX format/corridor-corner.fbx`
- `assets/Models/FBX format/corridor-end.fbx`
- `assets/Models/FBX format/corridor-intersection.fbx`
- `assets/Models/FBX format/corridor-junction.fbx`
- `assets/Models/FBX format/corridor-transition.fbx`
- `assets/Models/FBX format/corridor-wide-corner.fbx`
- `assets/Models/FBX format/corridor-wide-end.fbx`
- `assets/Models/FBX format/corridor-wide-intersection.fbx`
- `assets/Models/FBX format/corridor-wide-junction.fbx`
- `assets/Models/FBX format/corridor-wide.fbx`
- `assets/Models/FBX format/corridor.fbx`
- `assets/Models/FBX format/gate-door-window.fbx`
- `assets/Models/FBX format/gate-door.fbx`
- `assets/Models/FBX format/gate.fbx`
- `assets/Models/FBX format/gate.txt`
- `assets/Models/FBX format/room-corner.fbx`
- `assets/Models/FBX format/room-corner.txt`
- `assets/Models/FBX format/room-large-variation.fbx`
- `assets/Models/FBX format/room-large.fbx`
- `assets/Models/FBX format/room-small-variation.fbx`
- `assets/Models/FBX format/room-small.fbx`
- `assets/Models/FBX format/room-wide-variation.fbx`
- `assets/Models/FBX format/room-wide.fbx`
- `assets/Models/FBX format/stairs-wide.fbx`
- `assets/Models/FBX format/stairs.fbx`
- `assets/Models/FBX format/template-corner.fbx`
- `assets/Models/FBX format/template-detail.fbx`
- `assets/Models/FBX format/template-floor-big.fbx`
- `assets/Models/FBX format/template-floor-detail-a.fbx`
- `assets/Models/FBX format/template-floor-detail.fbx`
- `assets/Models/FBX format/template-floor-layer-raised.fbx`
- `assets/Models/FBX format/template-floor-layer.fbx`
- `assets/Models/FBX format/template-floor.fbx`
- `assets/Models/FBX format/template-wall-corner.fbx`
- `assets/Models/FBX format/template-wall-detail-a.fbx`
- `assets/Models/FBX format/template-wall-half.fbx`
- `assets/Models/FBX format/template-wall-stairs.fbx`
- `assets/Models/FBX format/template-wall-top.fbx`
- `assets/Models/FBX format/template-wall.fbx`
- `assets/Models/GLB format/corridor-corner.glb`
- `assets/Models/GLB format/corridor-end.glb`
- `assets/Models/GLB format/corridor-intersection.glb`
- `assets/Models/GLB format/corridor-junction.glb`
- `assets/Models/GLB format/corridor-transition.glb`
- `assets/Models/GLB format/corridor-wide-corner.glb`
- `assets/Models/GLB format/corridor-wide-end.glb`
- `assets/Models/GLB format/corridor-wide-intersection.glb`
- `assets/Models/GLB format/corridor-wide-junction.glb`
- `assets/Models/GLB format/corridor-wide.glb`
- `assets/Models/GLB format/corridor.glb`
- `assets/Models/GLB format/gate-door-window.glb`
- `assets/Models/GLB format/gate-door.glb`
- `assets/Models/GLB format/gate.glb`
- `assets/Models/GLB format/room-corner.glb`
- `assets/Models/GLB format/room-large-variation.glb`
- `assets/Models/GLB format/room-large.glb`
- `assets/Models/GLB format/room-small-variation.glb`
- `assets/Models/GLB format/room-small.glb`
- `assets/Models/GLB format/room-wide-variation.glb`
- `assets/Models/GLB format/room-wide.glb`
- `assets/Models/GLB format/stairs-wide.glb`
- `assets/Models/GLB format/stairs.glb`
- `assets/Models/GLB format/template-corner.glb`
- `assets/Models/GLB format/template-detail.glb`
- `assets/Models/GLB format/template-floor-big.glb`
- `assets/Models/GLB format/template-floor-detail-a.glb`
- `assets/Models/GLB format/template-floor-detail.glb`
- `assets/Models/GLB format/template-floor-layer-raised.glb`
- `assets/Models/GLB format/template-floor-layer.glb`
- `assets/Models/GLB format/template-floor.glb`
- `assets/Models/GLB format/template-wall-corner.glb`
- `assets/Models/GLB format/template-wall-detail-a.glb`
- `assets/Models/GLB format/template-wall-half.glb`
- `assets/Models/GLB format/template-wall-stairs.glb`
- `assets/Models/GLB format/template-wall-top.glb`
- `assets/Models/GLB format/template-wall.glb`
- `assets/Models/GLB format/Textures/colormap.png`
- `assets/Models/OBJ format/corridor-corner.mtl`
- `assets/Models/OBJ format/corridor-end.mtl`
- `assets/Models/OBJ format/corridor-intersection.mtl`
- `assets/Models/OBJ format/corridor-junction.mtl`
- `assets/Models/OBJ format/corridor-transition.mtl`
- `assets/Models/OBJ format/corridor-wide-corner.mtl`
- `assets/Models/OBJ format/corridor-wide-end.mtl`
- `assets/Models/OBJ format/corridor-wide-intersection.mtl`
- `assets/Models/OBJ format/corridor-wide-junction.mtl`
- `assets/Models/OBJ format/corridor-wide.mtl`
- `assets/Models/OBJ format/corridor.mtl`
- `assets/Models/OBJ format/gate-door-window.mtl`
- `assets/Models/OBJ format/gate-door.mtl`
- `assets/Models/OBJ format/gate.mtl`
- `assets/Models/OBJ format/room-corner.mtl`
- `assets/Models/OBJ format/room-large-variation.mtl`
- `assets/Models/OBJ format/room-large.mtl`
- `assets/Models/OBJ format/room-small-variation.mtl`
- `assets/Models/OBJ format/room-small.mtl`
- `assets/Models/OBJ format/room-wide-variation.mtl`
- `assets/Models/OBJ format/room-wide.mtl`
- `assets/Models/OBJ format/stairs-wide.mtl`
- `assets/Models/OBJ format/stairs.mtl`
- `assets/Models/OBJ format/template-corner.mtl`
- `assets/Models/OBJ format/template-detail.mtl`
- `assets/Models/OBJ format/template-floor-big.mtl`
- `assets/Models/OBJ format/template-floor-detail-a.mtl`
- `assets/Models/OBJ format/template-floor-detail.mtl`
- `assets/Models/OBJ format/template-floor-layer-raised.mtl`
- `assets/Models/OBJ format/template-floor-layer.mtl`
- `assets/Models/OBJ format/template-floor.mtl`
- `assets/Models/OBJ format/template-wall-corner.mtl`
- `assets/Models/OBJ format/template-wall-detail-a.mtl`
- `assets/Models/OBJ format/template-wall-half.mtl`
- `assets/Models/OBJ format/template-wall-stairs.mtl`
- `assets/Models/OBJ format/template-wall-top.mtl`
- `assets/Models/OBJ format/template-wall.mtl`
- `assets/Models/OBJ format/Textures/colormap.png`

</details>

### `assets/picture/UI1/` — 139 ファイル / 24.4 KB

UIパック1。`Divider Fade/divider-fade-002.png` のみ objectData で使用、他は未使用。

> ⚠ **`assets/picture/UI1/Divider Fade/divider-fade-002.png` は使用中**（objectData.json）。フォルダごと消す場合はこのファイルを別の場所へ移し、objectData.json のパスを書き換えること。

<details><summary>ファイル一覧</summary>

- `assets/picture/UI1/Border/panel-border-000.png`
- `assets/picture/UI1/Border/panel-border-001.png`
- `assets/picture/UI1/Border/panel-border-002.png`
- `assets/picture/UI1/Border/panel-border-003.png`
- `assets/picture/UI1/Border/panel-border-004.png`
- `assets/picture/UI1/Border/panel-border-005.png`
- `assets/picture/UI1/Border/panel-border-006.png`
- `assets/picture/UI1/Border/panel-border-007.png`
- `assets/picture/UI1/Border/panel-border-008.png`
- `assets/picture/UI1/Border/panel-border-009.png`
- `assets/picture/UI1/Border/panel-border-010.png`
- `assets/picture/UI1/Border/panel-border-011.png`
- `assets/picture/UI1/Border/panel-border-012.png`
- `assets/picture/UI1/Border/panel-border-013.png`
- `assets/picture/UI1/Border/panel-border-014.png`
- `assets/picture/UI1/Border/panel-border-015.png`
- `assets/picture/UI1/Border/panel-border-016.png`
- `assets/picture/UI1/Border/panel-border-017.png`
- `assets/picture/UI1/Border/panel-border-018.png`
- `assets/picture/UI1/Border/panel-border-019.png`
- `assets/picture/UI1/Border/panel-border-020.png`
- `assets/picture/UI1/Border/panel-border-021.png`
- `assets/picture/UI1/Border/panel-border-022.png`
- `assets/picture/UI1/Border/panel-border-023.png`
- `assets/picture/UI1/Border/panel-border-024.png`
- `assets/picture/UI1/Border/panel-border-025.png`
- `assets/picture/UI1/Border/panel-border-026.png`
- `assets/picture/UI1/Border/panel-border-027.png`
- `assets/picture/UI1/Border/panel-border-028.png`
- `assets/picture/UI1/Border/panel-border-029.png`
- `assets/picture/UI1/Border/panel-border-030.png`
- `assets/picture/UI1/Border/panel-border-031.png`
- `assets/picture/UI1/Divider/divider-000.png`
- `assets/picture/UI1/Divider/divider-001.png`
- `assets/picture/UI1/Divider/divider-002.png`
- `assets/picture/UI1/Divider/divider-003.png`
- `assets/picture/UI1/Divider/divider-004.png`
- `assets/picture/UI1/Divider/divider-005.png`
- `assets/picture/UI1/Divider Fade/divider-fade-000.png`
- `assets/picture/UI1/Divider Fade/divider-fade-001.png`
- `assets/picture/UI1/Divider Fade/divider-fade-003.png`
- `assets/picture/UI1/Divider Fade/divider-fade-004.png`
- `assets/picture/UI1/Divider Fade/divider-fade-005.png`
- `assets/picture/UI1/Panel/panel-000.png`
- `assets/picture/UI1/Panel/panel-001.png`
- `assets/picture/UI1/Panel/panel-002.png`
- `assets/picture/UI1/Panel/panel-003.png`
- `assets/picture/UI1/Panel/panel-004.png`
- `assets/picture/UI1/Panel/panel-005.png`
- `assets/picture/UI1/Panel/panel-006.png`
- `assets/picture/UI1/Panel/panel-007.png`
- `assets/picture/UI1/Panel/panel-008.png`
- `assets/picture/UI1/Panel/panel-009.png`
- `assets/picture/UI1/Panel/panel-010.png`
- `assets/picture/UI1/Panel/panel-011.png`
- `assets/picture/UI1/Panel/panel-012.png`
- `assets/picture/UI1/Panel/panel-013.png`
- `assets/picture/UI1/Panel/panel-014.png`
- `assets/picture/UI1/Panel/panel-015.png`
- `assets/picture/UI1/Panel/panel-016.png`
- `assets/picture/UI1/Panel/panel-017.png`
- `assets/picture/UI1/Panel/panel-018.png`
- `assets/picture/UI1/Panel/panel-019.png`
- `assets/picture/UI1/Panel/panel-020.png`
- `assets/picture/UI1/Panel/panel-021.png`
- `assets/picture/UI1/Panel/panel-022.png`
- `assets/picture/UI1/Panel/panel-023.png`
- `assets/picture/UI1/Panel/panel-024.png`
- `assets/picture/UI1/Panel/panel-025.png`
- `assets/picture/UI1/Panel/panel-026.png`
- `assets/picture/UI1/Panel/panel-027.png`
- `assets/picture/UI1/Panel/panel-028.png`
- `assets/picture/UI1/Panel/panel-029.png`
- `assets/picture/UI1/Panel/panel-030.png`
- `assets/picture/UI1/Panel/panel-031.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-000.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-001.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-002.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-003.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-004.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-005.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-006.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-007.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-008.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-009.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-010.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-011.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-012.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-013.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-014.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-015.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-016.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-017.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-018.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-019.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-020.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-021.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-022.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-023.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-024.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-025.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-026.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-027.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-028.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-029.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-030.png`
- `assets/picture/UI1/Transparent border/panel-transparent-border-031.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-000.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-001.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-002.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-003.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-004.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-005.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-006.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-007.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-008.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-009.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-010.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-011.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-012.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-013.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-014.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-015.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-016.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-017.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-018.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-019.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-020.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-021.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-022.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-023.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-024.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-025.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-026.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-027.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-028.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-029.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-030.png`
- `assets/picture/UI1/Transparent center/panel-transparent-center-031.png`

</details>

### `assets/picture/UI2/PNG/Double/` — 128 ファイル / 126.2 KB

UIパック2の2倍解像度版。未使用。

<details><summary>ファイル一覧</summary>

- `assets/picture/UI2/PNG/Double/banner_classic_curtain.png`
- `assets/picture/UI2/PNG/Double/banner_hanging.png`
- `assets/picture/UI2/PNG/Double/banner_modern.png`
- `assets/picture/UI2/PNG/Double/button_brown.png`
- `assets/picture/UI2/PNG/Double/button_brown_close.png`
- `assets/picture/UI2/PNG/Double/button_grey.png`
- `assets/picture/UI2/PNG/Double/button_grey_close.png`
- `assets/picture/UI2/PNG/Double/button_red.png`
- `assets/picture/UI2/PNG/Double/button_red_close.png`
- `assets/picture/UI2/PNG/Double/checkbox_beige_checked.png`
- `assets/picture/UI2/PNG/Double/checkbox_beige_cross.png`
- `assets/picture/UI2/PNG/Double/checkbox_beige_empty.png`
- `assets/picture/UI2/PNG/Double/checkbox_brown_checked.png`
- `assets/picture/UI2/PNG/Double/checkbox_brown_cross.png`
- `assets/picture/UI2/PNG/Double/checkbox_brown_empty.png`
- `assets/picture/UI2/PNG/Double/checkbox_grey_checked.png`
- `assets/picture/UI2/PNG/Double/checkbox_grey_cross.png`
- `assets/picture/UI2/PNG/Double/checkbox_grey_empty.png`
- `assets/picture/UI2/PNG/Double/hexagon_brown.png`
- `assets/picture/UI2/PNG/Double/hexagon_brown_dark.png`
- `assets/picture/UI2/PNG/Double/hexagon_damaged_brown.png`
- `assets/picture/UI2/PNG/Double/hexagon_damaged_brown_dark.png`
- `assets/picture/UI2/PNG/Double/hexagon_grey.png`
- `assets/picture/UI2/PNG/Double/hexagon_grey_dark.png`
- `assets/picture/UI2/PNG/Double/hexagon_grey_green.png`
- `assets/picture/UI2/PNG/Double/hexagon_grey_red.png`
- `assets/picture/UI2/PNG/Double/minimap_arrow_a.png`
- `assets/picture/UI2/PNG/Double/minimap_arrow_b.png`
- `assets/picture/UI2/PNG/Double/minimap_arrow_c.png`
- `assets/picture/UI2/PNG/Double/minimap_arrow_d.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_future_e.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_future_n.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_future_s.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_future_w.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_toon_e.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_toon_n.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_toon_s.png`
- `assets/picture/UI2/PNG/Double/minimap_compass_toon_w.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_exclamation_red.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_exclamation_white.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_exclamation_yellow.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_jewel_red.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_jewel_white.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_jewel_yellow.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_star_red.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_star_white.png`
- `assets/picture/UI2/PNG/Double/minimap_icon_star_yellow.png`
- `assets/picture/UI2/PNG/Double/minimap_ring_brown.png`
- `assets/picture/UI2/PNG/Double/minimap_ring_brown_detail.png`
- `assets/picture/UI2/PNG/Double/minimap_ring_grey.png`
- `assets/picture/UI2/PNG/Double/minimap_ring_grey_detail.png`
- `assets/picture/UI2/PNG/Double/minimap_ring_white.png`
- `assets/picture/UI2/PNG/Double/panel_border_brown.png`
- `assets/picture/UI2/PNG/Double/panel_border_brown_detail.png`
- `assets/picture/UI2/PNG/Double/panel_border_grey.png`
- `assets/picture/UI2/PNG/Double/panel_border_grey_detail.png`
- `assets/picture/UI2/PNG/Double/panel_brown.png`
- `assets/picture/UI2/PNG/Double/panel_brown_arrows.png`
- `assets/picture/UI2/PNG/Double/panel_brown_arrows_dark.png`
- `assets/picture/UI2/PNG/Double/panel_brown_arrows_dark_detail.png`
- `assets/picture/UI2/PNG/Double/panel_brown_arrows_detail.png`
- `assets/picture/UI2/PNG/Double/panel_brown_corners_a.png`
- `assets/picture/UI2/PNG/Double/panel_brown_corners_b.png`
- `assets/picture/UI2/PNG/Double/panel_brown_damaged.png`
- `assets/picture/UI2/PNG/Double/panel_brown_damaged_dark.png`
- `assets/picture/UI2/PNG/Double/panel_brown_dark.png`
- `assets/picture/UI2/PNG/Double/panel_brown_dark_corners_a.png`
- `assets/picture/UI2/PNG/Double/panel_brown_dark_corners_b.png`
- `assets/picture/UI2/PNG/Double/panel_grey.png`
- `assets/picture/UI2/PNG/Double/panel_grey_blue.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_blue.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_dark.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_detail_a.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_detail_b.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_green.png`
- `assets/picture/UI2/PNG/Double/panel_grey_bolts_red.png`
- `assets/picture/UI2/PNG/Double/panel_grey_dark.png`
- `assets/picture/UI2/PNG/Double/panel_grey_green.png`
- `assets/picture/UI2/PNG/Double/panel_grey_red.png`
- `assets/picture/UI2/PNG/Double/panel_grid_blueprint.png`
- `assets/picture/UI2/PNG/Double/panel_grid_paper.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_grey_large.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_grey_small.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_red_large.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_red_small.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_transparent_large.png`
- `assets/picture/UI2/PNG/Double/pattern_diagonal_transparent_small.png`
- `assets/picture/UI2/PNG/Double/pattern_grid_blueprint.png`
- `assets/picture/UI2/PNG/Double/pattern_grid_paper.png`
- `assets/picture/UI2/PNG/Double/progress_blue.png`
- `assets/picture/UI2/PNG/Double/progress_blue_border.png`
- `assets/picture/UI2/PNG/Double/progress_blue_small.png`
- `assets/picture/UI2/PNG/Double/progress_blue_small_border.png`
- `assets/picture/UI2/PNG/Double/progress_green.png`
- `assets/picture/UI2/PNG/Double/progress_green_border.png`
- `assets/picture/UI2/PNG/Double/progress_green_small.png`
- `assets/picture/UI2/PNG/Double/progress_green_small_border.png`
- `assets/picture/UI2/PNG/Double/progress_red.png`
- `assets/picture/UI2/PNG/Double/progress_red_border.png`
- `assets/picture/UI2/PNG/Double/progress_red_small.png`
- `assets/picture/UI2/PNG/Double/progress_red_small_border.png`
- `assets/picture/UI2/PNG/Double/progress_transparent.png`
- `assets/picture/UI2/PNG/Double/progress_transparent_small.png`
- `assets/picture/UI2/PNG/Double/progress_white.png`
- `assets/picture/UI2/PNG/Double/progress_white_border.png`
- `assets/picture/UI2/PNG/Double/progress_white_small.png`
- `assets/picture/UI2/PNG/Double/progress_white_small_border.png`
- `assets/picture/UI2/PNG/Double/round_brown.png`
- `assets/picture/UI2/PNG/Double/round_brown_dark.png`
- `assets/picture/UI2/PNG/Double/round_damaged_brown.png`
- `assets/picture/UI2/PNG/Double/round_damaged_brown_dark.png`
- `assets/picture/UI2/PNG/Double/round_grey.png`
- `assets/picture/UI2/PNG/Double/round_grey_dark.png`
- `assets/picture/UI2/PNG/Double/round_grey_detailed_green.png`
- `assets/picture/UI2/PNG/Double/round_grey_detailed_red.png`
- `assets/picture/UI2/PNG/Double/scrollbar_brown.png`
- `assets/picture/UI2/PNG/Double/scrollbar_brown_small.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_grey.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_grey_small.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_red.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_red_small.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_transparent.png`
- `assets/picture/UI2/PNG/Double/scrollbar_future_transparent_small.png`
- `assets/picture/UI2/PNG/Double/scrollbar_grey.png`
- `assets/picture/UI2/PNG/Double/scrollbar_grey_small.png`
- `assets/picture/UI2/PNG/Double/scrollbar_transparent.png`
- `assets/picture/UI2/PNG/Double/scrollbar_transparent_small.png`

</details>

### `assets/picture/UI2/Vector/` — 128 ファイル / 224.3 KB

UIパック2のSVG版。未使用。

<details><summary>ファイル一覧</summary>

- `assets/picture/UI2/Vector/banner_classic_curtain.svg`
- `assets/picture/UI2/Vector/banner_hanging.svg`
- `assets/picture/UI2/Vector/banner_modern.svg`
- `assets/picture/UI2/Vector/button_brown.svg`
- `assets/picture/UI2/Vector/button_brown_close.svg`
- `assets/picture/UI2/Vector/button_grey.svg`
- `assets/picture/UI2/Vector/button_grey_close.svg`
- `assets/picture/UI2/Vector/button_red.svg`
- `assets/picture/UI2/Vector/button_red_close.svg`
- `assets/picture/UI2/Vector/checkbox_beige_checked.svg`
- `assets/picture/UI2/Vector/checkbox_beige_cross.svg`
- `assets/picture/UI2/Vector/checkbox_beige_empty.svg`
- `assets/picture/UI2/Vector/checkbox_brown_checked.svg`
- `assets/picture/UI2/Vector/checkbox_brown_cross.svg`
- `assets/picture/UI2/Vector/checkbox_brown_empty.svg`
- `assets/picture/UI2/Vector/checkbox_grey_checked.svg`
- `assets/picture/UI2/Vector/checkbox_grey_cross.svg`
- `assets/picture/UI2/Vector/checkbox_grey_empty.svg`
- `assets/picture/UI2/Vector/hexagon_brown.svg`
- `assets/picture/UI2/Vector/hexagon_brown_dark.svg`
- `assets/picture/UI2/Vector/hexagon_damaged_brown.svg`
- `assets/picture/UI2/Vector/hexagon_damaged_brown_dark.svg`
- `assets/picture/UI2/Vector/hexagon_grey.svg`
- `assets/picture/UI2/Vector/hexagon_grey_dark.svg`
- `assets/picture/UI2/Vector/hexagon_grey_green.svg`
- `assets/picture/UI2/Vector/hexagon_grey_red.svg`
- `assets/picture/UI2/Vector/minimap_arrow_a.svg`
- `assets/picture/UI2/Vector/minimap_arrow_b.svg`
- `assets/picture/UI2/Vector/minimap_arrow_c.svg`
- `assets/picture/UI2/Vector/minimap_arrow_d.svg`
- `assets/picture/UI2/Vector/minimap_compass_future_e.svg`
- `assets/picture/UI2/Vector/minimap_compass_future_n.svg`
- `assets/picture/UI2/Vector/minimap_compass_future_s.svg`
- `assets/picture/UI2/Vector/minimap_compass_future_w.svg`
- `assets/picture/UI2/Vector/minimap_compass_toon_e.svg`
- `assets/picture/UI2/Vector/minimap_compass_toon_n.svg`
- `assets/picture/UI2/Vector/minimap_compass_toon_s.svg`
- `assets/picture/UI2/Vector/minimap_compass_toon_w.svg`
- `assets/picture/UI2/Vector/minimap_icon_exclamation_red.svg`
- `assets/picture/UI2/Vector/minimap_icon_exclamation_white.svg`
- `assets/picture/UI2/Vector/minimap_icon_exclamation_yellow.svg`
- `assets/picture/UI2/Vector/minimap_icon_jewel_red.svg`
- `assets/picture/UI2/Vector/minimap_icon_jewel_white.svg`
- `assets/picture/UI2/Vector/minimap_icon_jewel_yellow.svg`
- `assets/picture/UI2/Vector/minimap_icon_star_red.svg`
- `assets/picture/UI2/Vector/minimap_icon_star_white.svg`
- `assets/picture/UI2/Vector/minimap_icon_star_yellow.svg`
- `assets/picture/UI2/Vector/minimap_ring_brown.svg`
- `assets/picture/UI2/Vector/minimap_ring_brown_detail.svg`
- `assets/picture/UI2/Vector/minimap_ring_grey.svg`
- `assets/picture/UI2/Vector/minimap_ring_grey_detail.svg`
- `assets/picture/UI2/Vector/minimap_ring_white.svg`
- `assets/picture/UI2/Vector/panel_border_brown.svg`
- `assets/picture/UI2/Vector/panel_border_brown_detail.svg`
- `assets/picture/UI2/Vector/panel_border_grey.svg`
- `assets/picture/UI2/Vector/panel_border_grey_detail.svg`
- `assets/picture/UI2/Vector/panel_brown.svg`
- `assets/picture/UI2/Vector/panel_brown_arrows.svg`
- `assets/picture/UI2/Vector/panel_brown_arrows_dark.svg`
- `assets/picture/UI2/Vector/panel_brown_arrows_dark_detail.svg`
- `assets/picture/UI2/Vector/panel_brown_arrows_detail.svg`
- `assets/picture/UI2/Vector/panel_brown_corners_a.svg`
- `assets/picture/UI2/Vector/panel_brown_corners_b.svg`
- `assets/picture/UI2/Vector/panel_brown_damaged.svg`
- `assets/picture/UI2/Vector/panel_brown_damaged_dark.svg`
- `assets/picture/UI2/Vector/panel_brown_dark.svg`
- `assets/picture/UI2/Vector/panel_brown_dark_corners_a.svg`
- `assets/picture/UI2/Vector/panel_brown_dark_corners_b.svg`
- `assets/picture/UI2/Vector/panel_grey.svg`
- `assets/picture/UI2/Vector/panel_grey_blue.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_blue.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_dark.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_detail_a.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_detail_b.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_green.svg`
- `assets/picture/UI2/Vector/panel_grey_bolts_red.svg`
- `assets/picture/UI2/Vector/panel_grey_dark.svg`
- `assets/picture/UI2/Vector/panel_grey_green.svg`
- `assets/picture/UI2/Vector/panel_grey_red.svg`
- `assets/picture/UI2/Vector/panel_grid_blueprint.svg`
- `assets/picture/UI2/Vector/panel_grid_paper.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_grey_large.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_grey_small.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_red_large.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_red_small.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_transparent_large.svg`
- `assets/picture/UI2/Vector/pattern_diagonal_transparent_small.svg`
- `assets/picture/UI2/Vector/pattern_grid_blueprint.svg`
- `assets/picture/UI2/Vector/pattern_grid_paper.svg`
- `assets/picture/UI2/Vector/progress_blue.svg`
- `assets/picture/UI2/Vector/progress_blue_border.svg`
- `assets/picture/UI2/Vector/progress_blue_small.svg`
- `assets/picture/UI2/Vector/progress_blue_small_border.svg`
- `assets/picture/UI2/Vector/progress_green.svg`
- `assets/picture/UI2/Vector/progress_green_border.svg`
- `assets/picture/UI2/Vector/progress_green_small.svg`
- `assets/picture/UI2/Vector/progress_green_small_border.svg`
- `assets/picture/UI2/Vector/progress_red.svg`
- `assets/picture/UI2/Vector/progress_red_border.svg`
- `assets/picture/UI2/Vector/progress_red_small.svg`
- `assets/picture/UI2/Vector/progress_red_small_border.svg`
- `assets/picture/UI2/Vector/progress_transparent.svg`
- `assets/picture/UI2/Vector/progress_transparent_small.svg`
- `assets/picture/UI2/Vector/progress_white.svg`
- `assets/picture/UI2/Vector/progress_white_border.svg`
- `assets/picture/UI2/Vector/progress_white_small.svg`
- `assets/picture/UI2/Vector/progress_white_small_border.svg`
- `assets/picture/UI2/Vector/round_brown.svg`
- `assets/picture/UI2/Vector/round_brown_dark.svg`
- `assets/picture/UI2/Vector/round_damaged_brown.svg`
- `assets/picture/UI2/Vector/round_damaged_brown_dark.svg`
- `assets/picture/UI2/Vector/round_grey.svg`
- `assets/picture/UI2/Vector/round_grey_dark.svg`
- `assets/picture/UI2/Vector/round_grey_detailed_green.svg`
- `assets/picture/UI2/Vector/round_grey_detailed_red.svg`
- `assets/picture/UI2/Vector/scrollbar_brown.svg`
- `assets/picture/UI2/Vector/scrollbar_brown_small.svg`
- `assets/picture/UI2/Vector/scrollbar_future_grey.svg`
- `assets/picture/UI2/Vector/scrollbar_future_grey_small.svg`
- `assets/picture/UI2/Vector/scrollbar_future_red.svg`
- `assets/picture/UI2/Vector/scrollbar_future_red_small.svg`
- `assets/picture/UI2/Vector/scrollbar_future_transparent.svg`
- `assets/picture/UI2/Vector/scrollbar_future_transparent_small.svg`
- `assets/picture/UI2/Vector/scrollbar_grey.svg`
- `assets/picture/UI2/Vector/scrollbar_grey_small.svg`
- `assets/picture/UI2/Vector/scrollbar_transparent.svg`
- `assets/picture/UI2/Vector/scrollbar_transparent_small.svg`

</details>

### `assets/picture/UI2/Spritesheet/` — 4 ファイル / 160.0 KB

UIパック2のスプライトシート。未使用。

<details><summary>ファイル一覧</summary>

- `assets/picture/UI2/Spritesheet/spritesheet-default.png`
- `assets/picture/UI2/Spritesheet/spritesheet-default.xml`
- `assets/picture/UI2/Spritesheet/spritesheet-double.png`
- `assets/picture/UI2/Spritesheet/spritesheet-double.xml`

</details>

### `assets/mapdata/` — 1 ファイル / 0.3 KB

旧マップデータ。現在は `assets/data/mapData.json` を使用。

<details><summary>ファイル一覧</summary>

- `assets/mapdata/stage1.txt`

</details>

## B. 個別に削除してよいもの

使用中のファイルと同じフォルダにある未使用ファイル。

### BGM（`assets/sounds/BGMs/`） — 9 ファイル / 23.96 MB

| ファイル | サイズ | 備考 |
|---|---:|---|
| `assets/sounds/BGMs/dugeonBGM6.ogg` | 1.81 MB | ファイル名のtypo（dugeon） |
| `assets/sounds/BGMs/dungeonBGM1.ogg` | 2.89 MB | `dungeonBGM4.ogg` を使用中 |
| `assets/sounds/BGMs/dungeonBGM2.ogg` | 3.57 MB | `dungeonBGM4.ogg` を使用中 |
| `assets/sounds/BGMs/dungeonBGM3.ogg` | 5.50 MB | `dungeonBGM4.ogg` を使用中 |
| `assets/sounds/BGMs/dungeonBGM5.ogg` | 1.30 MB | `dungeonBGM4.ogg` を使用中 |
| `assets/sounds/BGMs/titleBGM.ogg` | 2.96 MB | `titleBGM2.ogg` を使用中 |
| `assets/sounds/BGMs/tonwBGM4.ogg` | 1.52 MB | ファイル名のtypo（tonw） |
| `assets/sounds/BGMs/townBGM.ogg` | 3.21 MB | `townBGM2.ogg` を使用中 |
| `assets/sounds/BGMs/townBGM3.ogg` | 1.20 MB | `townBGM2.ogg` を使用中 |

### SE（`assets/sounds/SEs/`） — 6 ファイル / 13.47 MB

| ファイル | サイズ | 備考 |
|---|---:|---|
| `assets/sounds/SEs/13 Fireworks_powerful explosions_multiples in a row_near.wav` | 13.30 MB | **約13MBで最大の不要ファイル** |
| `assets/sounds/SEs/clothes.mp3` | 19.9 KB |  |
| `assets/sounds/SEs/escape.mp3` | 10.7 KB | ESCAPE は `jump.mp3` を使用中 |
| `assets/sounds/SEs/escape2.mp3` | 32.6 KB | 同上 |
| `assets/sounds/SEs/escape3.mp3` | 70.3 KB | 同上 |
| `assets/sounds/SEs/escape4.mp3` | 39.8 KB | 同上 |

### 画像（`assets/picture/` 直下） — 4 ファイル / 1.91 MB

| ファイル | サイズ | 備考 |
|---|---:|---|
| `assets/picture/TitleBackground.png` | 1.91 MB | `TitleBackgroundLow.png` を使用中 |
| `assets/picture/fire.png` | 1.6 KB | パーティクルは `fire_ember.png` を使用中 |
| `assets/picture/fire_gaussian.png` | 1.6 KB | 同上 |
| `assets/picture/fire_glow.png` | 1.4 KB | 同上 |

### UIパック2（`assets/picture/UI2/PNG/Default/`） — 116 ファイル / 68.3 KB

全 131 ファイル中、使用中は以下の **15 ファイルのみ**。これ以外はすべて削除可。

- `checkbox_brown_checked.png`
- `minimap_arrow_a.png`
- `minimap_compass_future_e.png`
- `panel_border_grey_detail.png`
- `panel_brown.png`
- `panel_brown_damaged.png`
- `panel_grey.png`
- `panel_grey_bolts.png`
- `panel_grey_bolts_detail_a.png`
- `progress_green.png`
- `progress_green_black.png`
- `progress_red.png`
- `progress_red_black.png`
- `round_brown.png`
- `scrollbar_future_grey.png`

<details><summary>削除対象ファイル一覧</summary>

- `assets/picture/UI2/PNG/Default/banner_classic_curtain.png`
- `assets/picture/UI2/PNG/Default/banner_hanging.png`
- `assets/picture/UI2/PNG/Default/banner_modern.png`
- `assets/picture/UI2/PNG/Default/button_brown.png`
- `assets/picture/UI2/PNG/Default/button_brown_close.png`
- `assets/picture/UI2/PNG/Default/button_grey.png`
- `assets/picture/UI2/PNG/Default/button_grey_close.png`
- `assets/picture/UI2/PNG/Default/button_red.png`
- `assets/picture/UI2/PNG/Default/button_red_close.png`
- `assets/picture/UI2/PNG/Default/checkbox_beige_checked.png`
- `assets/picture/UI2/PNG/Default/checkbox_beige_cross.png`
- `assets/picture/UI2/PNG/Default/checkbox_beige_empty.png`
- `assets/picture/UI2/PNG/Default/checkbox_brown_cross.png`
- `assets/picture/UI2/PNG/Default/checkbox_brown_empty.png`
- `assets/picture/UI2/PNG/Default/checkbox_grey_checked.png`
- `assets/picture/UI2/PNG/Default/checkbox_grey_cross.png`
- `assets/picture/UI2/PNG/Default/checkbox_grey_empty.png`
- `assets/picture/UI2/PNG/Default/hexagon_brown.png`
- `assets/picture/UI2/PNG/Default/hexagon_brown_dark.png`
- `assets/picture/UI2/PNG/Default/hexagon_damaged_brown.png`
- `assets/picture/UI2/PNG/Default/hexagon_damaged_brown_dark.png`
- `assets/picture/UI2/PNG/Default/hexagon_grey.png`
- `assets/picture/UI2/PNG/Default/hexagon_grey_dark.png`
- `assets/picture/UI2/PNG/Default/hexagon_grey_green.png`
- `assets/picture/UI2/PNG/Default/hexagon_grey_red.png`
- `assets/picture/UI2/PNG/Default/minimap_arrow_b.png`
- `assets/picture/UI2/PNG/Default/minimap_arrow_c.png`
- `assets/picture/UI2/PNG/Default/minimap_arrow_d.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_future_n.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_future_s.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_future_w.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_toon_e.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_toon_n.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_toon_s.png`
- `assets/picture/UI2/PNG/Default/minimap_compass_toon_w.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_exclamation_red.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_exclamation_white.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_exclamation_yellow.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_jewel_red.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_jewel_white.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_jewel_yellow.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_star_red.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_star_white.png`
- `assets/picture/UI2/PNG/Default/minimap_icon_star_yellow.png`
- `assets/picture/UI2/PNG/Default/minimap_ring_brown.png`
- `assets/picture/UI2/PNG/Default/minimap_ring_brown_detail.png`
- `assets/picture/UI2/PNG/Default/minimap_ring_grey.png`
- `assets/picture/UI2/PNG/Default/minimap_ring_grey_detail.png`
- `assets/picture/UI2/PNG/Default/minimap_ring_white.png`
- `assets/picture/UI2/PNG/Default/panel_border_brown.png`
- `assets/picture/UI2/PNG/Default/panel_border_brown_detail.png`
- `assets/picture/UI2/PNG/Default/panel_border_grey.png`
- `assets/picture/UI2/PNG/Default/panel_brown_arrows.png`
- `assets/picture/UI2/PNG/Default/panel_brown_arrows_dark.png`
- `assets/picture/UI2/PNG/Default/panel_brown_arrows_dark_detail.png`
- `assets/picture/UI2/PNG/Default/panel_brown_arrows_detail.png`
- `assets/picture/UI2/PNG/Default/panel_brown_corners_a.png`
- `assets/picture/UI2/PNG/Default/panel_brown_corners_b.png`
- `assets/picture/UI2/PNG/Default/panel_brown_damaged_dark.png`
- `assets/picture/UI2/PNG/Default/panel_brown_dark.png`
- `assets/picture/UI2/PNG/Default/panel_brown_dark_corners_a.png`
- `assets/picture/UI2/PNG/Default/panel_brown_dark_corners_b.png`
- `assets/picture/UI2/PNG/Default/panel_grey_blue.png`
- `assets/picture/UI2/PNG/Default/panel_grey_bolts_blue.png`
- `assets/picture/UI2/PNG/Default/panel_grey_bolts_dark.png`
- `assets/picture/UI2/PNG/Default/panel_grey_bolts_detail_b.png`
- `assets/picture/UI2/PNG/Default/panel_grey_bolts_green.png`
- `assets/picture/UI2/PNG/Default/panel_grey_bolts_red.png`
- `assets/picture/UI2/PNG/Default/panel_grey_dark.png`
- `assets/picture/UI2/PNG/Default/panel_grey_green.png`
- `assets/picture/UI2/PNG/Default/panel_grey_red.png`
- `assets/picture/UI2/PNG/Default/panel_grid_blueprint.png`
- `assets/picture/UI2/PNG/Default/panel_grid_paper.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_grey_large.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_grey_small.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_red_large.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_red_small.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_transparent_large.png`
- `assets/picture/UI2/PNG/Default/pattern_diagonal_transparent_small.png`
- `assets/picture/UI2/PNG/Default/pattern_grid_blueprint.png`
- `assets/picture/UI2/PNG/Default/pattern_grid_paper.png`
- `assets/picture/UI2/PNG/Default/progress_black.png`
- `assets/picture/UI2/PNG/Default/progress_blue.png`
- `assets/picture/UI2/PNG/Default/progress_blue_border.png`
- `assets/picture/UI2/PNG/Default/progress_blue_small.png`
- `assets/picture/UI2/PNG/Default/progress_blue_small_border.png`
- `assets/picture/UI2/PNG/Default/progress_green_border.png`
- `assets/picture/UI2/PNG/Default/progress_green_small.png`
- `assets/picture/UI2/PNG/Default/progress_green_small_border.png`
- `assets/picture/UI2/PNG/Default/progress_red_border.png`
- `assets/picture/UI2/PNG/Default/progress_red_small.png`
- `assets/picture/UI2/PNG/Default/progress_red_small_border.png`
- `assets/picture/UI2/PNG/Default/progress_transparent.png`
- `assets/picture/UI2/PNG/Default/progress_transparent_small.png`
- `assets/picture/UI2/PNG/Default/progress_white.png`
- `assets/picture/UI2/PNG/Default/progress_white_border.png`
- `assets/picture/UI2/PNG/Default/progress_white_small.png`
- `assets/picture/UI2/PNG/Default/progress_white_small_border.png`
- `assets/picture/UI2/PNG/Default/round_brown_dark.png`
- `assets/picture/UI2/PNG/Default/round_damaged_brown.png`
- `assets/picture/UI2/PNG/Default/round_damaged_brown_dark.png`
- `assets/picture/UI2/PNG/Default/round_grey.png`
- `assets/picture/UI2/PNG/Default/round_grey_dark.png`
- `assets/picture/UI2/PNG/Default/round_grey_detailed_green.png`
- `assets/picture/UI2/PNG/Default/round_grey_detailed_red.png`
- `assets/picture/UI2/PNG/Default/scrollbar_brown.png`
- `assets/picture/UI2/PNG/Default/scrollbar_brown_small.png`
- `assets/picture/UI2/PNG/Default/scrollbar_future_grey_small.png`
- `assets/picture/UI2/PNG/Default/scrollbar_future_red.png`
- `assets/picture/UI2/PNG/Default/scrollbar_future_red_small.png`
- `assets/picture/UI2/PNG/Default/scrollbar_future_transparent.png`
- `assets/picture/UI2/PNG/Default/scrollbar_future_transparent_small.png`
- `assets/picture/UI2/PNG/Default/scrollbar_grey.png`
- `assets/picture/UI2/PNG/Default/scrollbar_grey_small.png`
- `assets/picture/UI2/PNG/Default/scrollbar_transparent.png`
- `assets/picture/UI2/PNG/Default/scrollbar_transparent_small.png`

</details>

### UIパック2 付属ファイル（`assets/picture/UI2/` 直下） — 5 ファイル / 78.4 KB

| ファイル | サイズ | 備考 |
|---|---:|---|
| `assets/picture/UI2/License.txt` | 0.7 KB | CC0ライセンスのため表記義務はないが、残すかは任意 |
| `assets/picture/UI2/Preview.png` | 47.7 KB |  |
| `assets/picture/UI2/Sample.png` | 30.0 KB |  |
| `assets/picture/UI2/Visit Kenney.url` | 0.0 KB |  |
| `assets/picture/UI2/Visit Patreon.url` | 0.1 KB |  |

### 3Dモデルのメモ（`assets/3DModels/rockObj/`） — 3 ファイル / 1.7 KB

| ファイル | サイズ | 備考 |
|---|---:|---|
| `assets/3DModels/rockObj/rockFloor.txt` | 0.6 KB | テキストメモ。ゲームからは読み込まれない |
| `assets/3DModels/rockObj/rockRoof.txt` | 0.6 KB | テキストメモ。ゲームからは読み込まれない |
| `assets/3DModels/rockObj/rockWall.txt` | 0.5 KB | テキストメモ。ゲームからは読み込まれない |

## C. 要確認（コードから参照はないが、消す前に判断が必要）

| ファイル | 備考 |
|---|---|
| `assets/data/ReadMe.txt` | 開発用メモ（敵追加手順）。ゲームでは不要 |
| `assets/data/itamData_init.json` | セーブデータ初期値のテンプレートと思われる（ファイル名typo：itam）。配布前に `itemData.json` をこの内容でリセットする用途なら、リセット後に削除可 |
| `assets/data/playerData_init.json` | 同上（`playerData.json` の初期値） |
| `assets/data/resourceData_init.json` | 同上（`resourceData.json` の初期値）。現在の `resourceData.json` と GOLD の `defaultYield` が異なる（init: 1 / 現行: 0）ので注意 |
| `assets/shader/hlsl/*.hlsl, *.hlsli`（14ファイル） | シェーダーのソース。実行時は `assets/shader/cso/*.cso` のみ読み込むので**配布物には不要**。ただしビルドに必要なのでリポジトリからは消さないこと |

## 補足

### コード上で参照されているが存在しないファイル

- `assets/data/spriteData.json`（`SpriteComponent::loadFileAndCreate`）
- `assets/data/textData.json`（`TextComponent::loadFileAndCreate`）

どちらの関数も呼び出し元がないため実害はない。不要なら関数ごと削除してよい。

### assets 以外で配布物に含めなくてよいもの

- `x64/`, `DirectX12/x64/`, `Debug/`, `DirectX12/Debug/` … ビルド中間生成物（`DirectX12/x64/` だけで約1GB）
- `DirectX12/imgui.ini` … ImGui のウィンドウ配置保存ファイル
- `DirectX12/DirectX12.vcxproj.user` … VSのユーザー設定
- `Claude outputs/` … 開発用の出力物

### 使用中と判定したもの（参考）

- `assets/3DModels/` の全 .fbx / .json（meshData.json に登録された全メッシュはコードまたは objectData から使用されている）
- `assets/texture/` の全5ファイル（モデルJSONの texturePath から参照）
- `assets/shader/cso/` の全11ファイル（Graphic.cpp から参照）
- `assets/data/` の上記C以外のJSON

> 注：判定はパス文字列の静的検索によるもの。文字列連結などで動的にパスを組み立てている箇所は `FBXConverter.cpp` の `"assets/texture/" + textureFilename` のみで、これはモデルJSONの texturePath で検証済み。削除後は一通りプレイして確認することを推奨。
