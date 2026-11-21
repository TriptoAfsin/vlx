# VLX

**VLX is a personal, open source fork of [VLC media player](https://www.videolan.org/vlc/)**
with a more modern, fluid UI and tuned defaults. Everything that makes VLC great (the engine,
the codecs, "plays everything") is VLC's work. VLX only reskins and tunes it.

> VLX is **not** affiliated with or endorsed by VideoLAN. "VLC" and the cone logo are
> trademarks of VideoLAN. VLX uses its own name and icon for that reason.

## Goals

1. **UI first.** A calmer, more modern Qt/QML interface: consistent spacing, softer surfaces,
   smooth transitions, a refreshed player view and media library.
2. **Performance second.** Faster startup and snappier UI through build flags and defaults
   (LTO, lazy-loaded views, sensible hardware-decoding defaults), never by dropping features.
3. **Stay close to upstream.** Small, contained diffs so VLX can keep rebasing on VLC `master`.

Features beyond that come later.

## Status

Early. The repo is initialized, and the design direction and the icon are being chosen.
See [`DESIGN.md`](DESIGN.md) once the direction is locked.

## Building

VLX builds the same way VLC does. On Windows that means a mingw-w64 toolchain inside WSL, msys2
or Docker. Start with [`doc/BUILD-win32.md`](doc/BUILD-win32.md). [`VERIFY.md`](VERIFY.md) lists
the exact commands used to check a change.

```sh
# WSL / Linux, cross-compiling for Windows x64, with prebuilt contribs
./extras/package/win32/build.sh -a x86_64 -p -m
```

## Branches and remotes

| Ref | What it is |
|---|---|
| `main` | VLX: upstream VLC plus the VLX changes |
| `master` | Untouched mirror of `upstream/master` |
| `upstream` remote | `https://github.com/videolan/vlc.git` |
| `origin` remote | `https://github.com/TriptoAfsin/vlx.git` |

To sync: `git fetch upstream && git rebase upstream/master` on `main`.

## Credits and license

All credit for the media engine, the codecs and the original interface goes to the
[VideoLAN](https://www.videolan.org) team and VLC's contributors (see [`AUTHORS`](AUTHORS) and
[`THANKS`](THANKS)). The original VLC README is kept at [`README.VLC.md`](README.VLC.md).

VLX keeps VLC's licensing: the application is **GPLv2 or later** ([`COPYING`](COPYING)), and libVLC is
**LGPLv2.1 or later** ([`COPYING.LIB`](COPYING.LIB)). Every VLX change is released under the same terms.
