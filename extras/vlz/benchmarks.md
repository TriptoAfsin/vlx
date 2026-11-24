# VLZ benchmarks

Measured with [`scripts/bench.ps1`](scripts/bench.ps1) (VERIFY.md §4). Each player is a portable copy with its own
throwaway profile and a generated plugin cache (`vlc-cache-gen`), run one at a time on the same PC.

## 2026-11-24, GDI font engine (latest)

Same PC, sample and method as the Lite mode run below. VLZ 0.2.1: Qt uses the GDI font database instead of
DirectWrite's in both modes, and the play queue panel loads when first opened.

| Metric | VLC 3.0.24 | VLC 4.0 nightly | VLZ | VLZ Lite |
|---|---:|---:|---:|---:|
| Install size (MB) | 188.2 | 254.5 | 177.3 | 177.3 |
| Start-up, first (ms) | 136 | 725 | 546 | 320 |
| Start-up, warm median (ms) | 100 | 416 | 367 | 345 |
| Idle private (MB) | 10.3 | 138.8 | **119.0** | **42.4** |
| Idle working set (MB) | 36.5 | 176.7 | 120.4 | 74.0 |
| Playing private (MB) | 236.2 | 307.8 | **293.6** | **216.9** |
| Playing peak working set (MB) | 232.2 | 350.1 | 287.9 | 230.8 |
| Playing CPU (% of all cores) | 0.2 | 0.3 | 0.3 | 0.2 |
| UI ready, second pass (ms) | 142* | – | 684 | 400 |

\* from the 2026-11-23 run.

How to read it:

- **VLZ now beats the VLC 4 nightly on every line**, and **Lite uses less memory than VLC 3 while playing**
  (216.9 vs 236.2 MB, peak working set 230.8 vs 232.2 MB).
- Against 0.2.0: VLZ idle 144.7 → 119.0 MB and working set 175.9 → 120.4 MB; Lite idle 67.9 → 42.4 MB and
  working set 129.9 → 74.0 MB; UI ready 916 → 684 ms and 611 → 400 ms.
- Why: Qt 6's DirectWrite font database opens and maps every installed font file at start-up (394 files,
  529 MB here) plus Windows' 85 MB shared font cache. GDI's asks Windows for names and opens a font when it is
  drawn (8 files here). VLC 3's Qt 5 already worked that way.
- Playback cost checked on 25 s runs (CPU as % of one core, dropped frames): 1080p fullscreen with controls
  always shown, VLZ 7.0 % / 2, Lite 5.2 % / 0. Audio with the spinning turntable skin, VLZ 7.0 %, Lite 25.4 %:
  the software renderer costs CPU when the interface animates. Video decoding stays on the GPU in both modes.
- What's left, measured with a timed log: libvlc 28 ms, Qt start 70 ms, loading the QML interface about 245 ms,
  first frames about 120 ms. QML can't be compiled ahead of time while VLC registers 130 types at run time.

## 2026-11-23, Lite mode

Same PC and sample, 20 s of playback, start-up median of the last 11 of 12 launches. VLZ 0.1.2 plus Lite mode
(Settings › Performance), one build: "VLZ Lite" is the same folder started with `--qt-vlz-lite`
(`bench.ps1 -App "VLZ Lite=<folder>|--qt-vlz-lite"`), which is what the setting does at start-up.

| Metric | VLC 3.0.24 | VLC 4.0 nightly | VLZ | VLZ Lite |
|---|---:|---:|---:|---:|
| Install size (MB) | 188.2 | 254.5 | 177.3 | 177.3 |
| Plugins | 360 | 410 | 385 | 385 |
| Start-up, first (ms) | 167 | 888 | 640 | 444 |
| Start-up, warm median (ms) | 135 | 481 | 458 | 456 |
| Idle private (MB) | 10.4 | 137.1 | 144.7 | **67.9** |
| Idle working set (MB) | 36.9 | 174.5 | 175.9 | 129.9 |
| Playing private (MB) | 234.6 | 308.5 | 314.5 | **239.0** |
| Playing peak working set (MB) | 231.9 | 362.4 | 341.6 | 289.4 |
| Playing CPU (% of all cores) | 0.2 | 0.2 | 0.3 | 0.3 |

`bench.ps1` times start-up to the main window *existing*, which happens before the interface has loaded, so it
hides Lite's gain. A second pass timed **UI ready** (window exists and the process has used no CPU for 200 ms),
median of the last 9 of 10 launches:

| | VLC 3.0.24 | VLZ | VLZ Lite |
|---|---:|---:|---:|
| UI ready (ms) | 142 | 916 | **611** |
| vlc-preparser helper while playing (MB) | 0 | 3.5 | 0 |

How to read it:

- **Lite halves idle memory** (145 → 68 MB) and brings playing memory level with VLC 3 (239 vs 235 MB), with
  the same interface. The UI is ready a third sooner.
- Where it comes from, each measured on its own against the default (idle / playing private MB):
  software renderer for the interface 144 → 80 / 321 → 282; Memory saver − / 317 → 275; no media library
  145 → 136 / −; no background parsing: no helper process. The rest of the gap to VLC 3's idle 10 MB is Qt 6's
  QML engine and the interface itself.
- Leads measured and dropped (within noise): no backdrop blur, no tray icon, no Lua extensions, the basic render
  loop, the QML interpreter. The OpenGL backend costs more (idle 203 MB, playing 438 MB). Minimal view on top of
  Lite reaches idle 93 / playing 222 MB and UI ready 371 ms, but hides the whole interface, so it isn't part of Lite.

## 2026-11-22, precompiled QML

Same PC and sample, 20 s of playback, start-up median of the last 11 of 12 launches. Changes since the first run:
QML precompiled with `qmlcachegen` (the Meson build didn't do it; the nightly's autotools build does), the command
palette loads on first Ctrl+K, network discovery in the media library is off by default (no libupnp at launch), and
the libgme plugin loads (it needed `libgcc_s_seh-1.dll`, which nobody ships; the nightly still fails to load it).

| Metric | VLC 4.0 nightly `7f0d5b95` | VLZ |
|---|---:|---:|
| Install size (MB) | 254.5 | 176.9 |
| Without translations (MB) | 220.7 | 176.9 |
| Plugins | 410 | 385 |
| Plugins that fail to load | 4 | 0 |
| Start-up, first (ms) | 649 | 625 |
| Start-up, warm median (ms) | 428 | 417 |
| Idle private (MB) | 136.1 | 136.9 |
| Playing private (MB) | 306.6 | 306.4 |
| Playing peak working set (MB) | 352.5 | 335.3 |
| Playing CPU (% of all cores) | 0.3 | 0.3 |

VLZ now starts slightly faster than the nightly. A timed `-vv` log puts the interface load (Qt window to QML ready) at
293 ms against the nightly's 292 ms, down from 319 ms without precompiled QML. The size went up 3.3 MB from the
compiled QML.

## 2026-11-22, first local release build

PC: 24 logical cores, NVIDIA GeForce RTX 5070, Windows 11, 3840 × 2160 at 150 %.
Sample: Big Buck Bunny 1080p H.264 clip, 45 s of looped playback. Start-up: 6 launches, median of the last 5.

| Metric | VLC 3.0.24 (stable) | VLC 4.0 nightly `7f0d5b95` | VLZ |
|---|---:|---:|---:|
| Install size (MB) | 188.2 | 254.5 | 173.6 |
| Without translations (MB) | 146.3 | 220.7 | 173.6 |
| Files | 585 | 630 | 641 |
| Plugins | 360 | 410 | 385 |
| Start-up, first (ms) | 255 | 628 | 670 |
| Start-up, warm median (ms) | 91 | 444 | 499 |
| Idle private (MB) | 10.2 | 136.4 | 137.6 |
| Idle working set (MB) | 36.6 | 172.2 | 173.4 |
| Playing private (MB) | 176.9 | 312.0 | 318.2 |
| Playing peak working set (MB) | 242.5 | 344.7 | 330.0 |
| Playing CPU (% of all cores) | 0.1 | 0.3 | 0.3 |

How to read it:

- **The fair comparison is VLZ against the VLC 4 nightly.** The nightly is the commit right before VLZ's upstream base.
  VLC 3.0.24 has a different, much lighter interface (Qt 5 widgets instead of Qt 6 QML), which explains most of its
  lead in start-up time and idle memory. That cost comes with VLC 4's interface, not with VLZ.
- **Size:** VLZ is 21 % smaller than the nightly without translations (173.6 vs 220.7 MB), from the feature cuts in
  `vlz.ini`: 385 plugins instead of 410. VLZ doesn't ship translations yet (`-l` isn't passed to `build.sh`).
- **Memory and CPU** are within noise of the nightly; all three decode with D3D11VA on the GPU.
- **Start-up** is about 55 ms slower than the nightly. Single runs vary, so treat this as a lead to check, not a result.
  (Found and fixed: the QML wasn't precompiled. See the run above.)
- VLZ's build here is the local WSL build (GCC 13, MinGW runtime linked statically); the release workflow builds with
  VideoLAN's GCC 14 image. Neither uses LTO yet.
