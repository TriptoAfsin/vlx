# VLX roadmap

Found by auditing VLC 4.0-dev at fork time. Paths are relative to `modules/gui/qt/` unless they start at the repo root.
Status: **planned**. The UI items wait for the design direction in `DESIGN.md`.

## 1. UI (first priority)

Ordered by leverage against diff size. Most are one file each, which keeps rebases cheap.

| # | Change | Where | Size |
|---|---|---|---|
| 1 | New palette: neutral ramp, accent, hover and selection states (many current values are marked `FIXME`) | `style/systempalette.hpp:114-136`, `style/systempalette.cpp` (`makeLightPalette` 470, `makeDarkPalette` 684) | S |
| 2 | Retune tokens: radii 4 → 8-10, durations 100/200 → 150/250, add easing tokens (each component hard-codes Sine curves today) | `style/VLCStyle.qml` | S |
| 3 | Page transitions: every view switch is instant because `replaceEnter`/`replaceExit` are `null` | `widgets/qml/StackViewExt.qml:43-45` | S |
| 4 | Crossfade between library and player (a plain Loader swap today) | `maininterface/qml/MainInterface.qml:298-323` | S-M |
| 5 | Control bar auto-hide: fade plus a short slide, with out/in cubic easing (opacity only today) | `util/qml/FadeControllerStateGroup.qml` | S |
| 6 | Sidebar and playlist: slide with a transform, not a 100 ms `width` animation that relayouts every frame | `maininterface/qml/MainDisplay.qml:690-785`, `player/qml/PlayerView.qml:1079-1092` | M |
| 7 | Grid hover: rounded background, a small lift and a shadow | `widgets/qml/GridItem.qml:252-275` | S |
| 8 | Windows 11: Mica / Mica Alt backdrop instead of Acrylic, and rounded window corners (`DWMWA_WINDOW_CORNER_PREFERENCE` is unused) | `maininterface/win32windoweffects_module.cpp:69-86` | S |
| 9 | UI font: bundle one (none today, so Qt falls back to Segoe UI) and maybe 12 → 13 base size | `qt.cpp` near 1255, `assets.qrc` | S |
| 10 | Mouse-hide timeout 1 s → 2-2.5 s, so controls don't vanish too fast | `src/libvlc-module.c:1720` | XS |

**Later:** the preferences, Open and Media Info dialogs are still QWidgets (`dialogs/preferences/*`). Restyling them
means a Qt widget stylesheet or porting them to QML. That's a big job, so it stays out of the first pass.

## 2. Branding

User-facing only. Binary names, config paths and `PACKAGE` stay as they are so plugins, settings and rebases keep working.

- Icons: `share/icons/**` (feeds `assets.qrc:2-11`), `share/icons/vlc.ico`, `share/vlc512x512.png`
- In-app cone: `pixmaps/misc/cone.svg`, used by `BannerCone.qml`, `NoMedialibHome.qml`, `MinimalView.qml`, `UpdatePane.qml`
- Holiday cone variants: `qt.cpp:1258-1261`
- Window title: `qt.cpp:1255` (`setApplicationDisplayName`)
- Windows resources and installer: `bin/vlc_win32_rc.rc.in`, `extras/package/win32/NSIS/vlc.win32.nsi.in`, `extras/package/win32/msi/product.wxs`

## 3. Performance (second priority)

Hardware decoding (d3d11va) and the Direct3D 11 video output are already the defaults on Windows, so the wins are
startup and UI smoothness.

| Change | Expected impact | Risk |
|---|---|---|
| Compile QML ahead of time under Meson (`qmlcachegen`/`qmlsc`). `meson.build:1271` is a TODO; autotools already does it | High: cold-start QML compile, likely hundreds of ms | Build work only |
| Release builds with LTO and `-O3` (`build.sh -r -t`, or `-Db_lto=true -Doptimization=3`) | Low to medium CPU win | Low |
| Async-load the player view and preload it when idle | Faster first switch to the player | Low-medium (focus handling) |
| `ml-network-discovery` off by default; start the first library scan after the first frame | Less startup I/O | Low |
| PGO with clang `-fprofile-use` | 5-15% on hot paths | High upkeep |

Each one gets before and after numbers, following `VERIFY.md` §5.

## 4. Features (far future)

Not planned yet. Ideas so far: a command palette (Ctrl K), resume-position sync, a better subtitle search flow.
