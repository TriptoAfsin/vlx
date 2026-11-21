# VLX benchmarks

Measured with [`scripts/bench.ps1`](scripts/bench.ps1) (VERIFY.md §4). Each player is a portable copy with its own
throwaway profile and a generated plugin cache (`vlc-cache-gen`), run one at a time on the same PC.

## 2026-11-22, first local release build

PC: 24 logical cores, NVIDIA GeForce RTX 5070, Windows 11, 3840 × 2160 at 150 %.
Sample: Big Buck Bunny 1080p H.264 clip, 45 s of looped playback. Start-up: 6 launches, median of the last 5.

| Metric | VLC 3.0.24 (stable) | VLC 4.0 nightly `7f0d5b95` | VLX |
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

- **The fair comparison is VLX against the VLC 4 nightly.** The nightly is the commit right before VLX's upstream base.
  VLC 3.0.24 has a different, much lighter interface (Qt 5 widgets instead of Qt 6 QML), which explains most of its
  lead in start-up time and idle memory. That cost comes with VLC 4's interface, not with VLX.
- **Size:** VLX is 21 % smaller than the nightly without translations (173.6 vs 220.7 MB), from the feature cuts in
  `vlx.ini`: 385 plugins instead of 410. VLX doesn't ship translations yet (`-l` isn't passed to `build.sh`).
- **Memory and CPU** are within noise of the nightly; all three decode with D3D11VA on the GPU.
- **Start-up** is about 55 ms slower than the nightly. Single runs vary, so treat this as a lead to check, not a result.
- VLX's build here is the local WSL build (GCC 13, MinGW runtime linked statically); the release workflow builds with
  VideoLAN's GCC 14 image. Neither uses LTO yet.
