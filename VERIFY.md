# VERIFY.md

How to check a VLX change before calling it done. Run the steps that apply to what changed.

## 0. Environment

VLX builds on Windows with a mingw-w64 toolchain inside **WSL (Ubuntu)**. That's the recommended route, and the
dev machine already has `Ubuntu-26.04` under WSL 2. Msys2 and Docker also work (see `doc/BUILD-win32.md`).

One-time setup inside WSL: install the packages listed in `doc/BUILD-win32.md` (Ubuntu section), plus
`llvm-mingw` if you want PDBs for debugging.

Build from a Linux filesystem path (e.g. `~/src/vlx`), not `/mnt/l/...`. Builds on the Windows drive are many times slower.

## 1. Build

```sh
# Debug build, prebuilt contribs, Meson
./extras/package/win32/build.sh -a x86_64 -p -m

# Release build with LTO (what VLX ships)
./extras/package/win32/build.sh -a x86_64 -p -m -r -t
```

Pass: the build finishes with no new warnings in the files you touched.

## 2. Tests

```sh
# From the build directory
meson test --suite qt          # Qt/QML model and component tests (modules/gui/qt/tests)
meson test                     # whole suite, before merging larger changes
```

Pass: no new failures compared with `master`.

## 3. QML

For QML changes, also:
- Run `qmllint` on the changed `.qml` files (it ships with Qt 6). No new warnings.
- Launch with `--verbose=2` and check the log for QML errors and binding loops.

## 4. Run it

Run the built `vlc.exe` from the build's install directory on Windows and check by hand:

- [ ] App starts. Note the cold-start time to the first frame of the UI.
- [ ] Play a local file. Seek, pause, change volume, toggle fullscreen.
- [ ] Open the playlist and the media library. Switch between grid and list.
- [ ] Light and dark theme both look right (Settings → Interface → colour scheme).
- [ ] Window resize, maximize and snap work. Client-side decorations aren't broken.
- [ ] Changed screens animate smoothly, with no jumps or flicker.

For UI changes, take before/after screenshots of each changed screen and attach them to the PR.

## 5. Performance changes

Measure before and after, on the same machine and file, and report both numbers:

- **Startup:** time from launch to the first UI frame, median of 5 runs, cold and warm.
- **Playback:** CPU and GPU use while playing a 4K HEVC file (Task Manager or `perfmon`), and dropped frames
  (Tools → Media Information → Statistics).
- **UI:** frame rate while scrolling a large media library grid (`QSG_RENDER_TIMING=1` prints frame timings).

A performance change without numbers isn't done.
