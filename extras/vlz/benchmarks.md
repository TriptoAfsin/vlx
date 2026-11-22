# VLZ benchmarks

Measured with [`scripts/bench.ps1`](scripts/bench.ps1) (VERIFY.md §4). Each player is a portable copy with its own
throwaway profile and a generated plugin cache (`vlc-cache-gen`), run one at a time on the same PC.

## 2026-11-22, precompiled QML (latest)

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
