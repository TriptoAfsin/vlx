# VERIFY.md

What "checked" means for each kind of VLX change. A change is committed only after it builds cleanly and passes
the checks for its kind. Modelled on tTorrent's flow.

## 0. Environment (once per machine)

VLX builds with a mingw-w64 toolchain inside **WSL (Ubuntu)**. The machine has `Ubuntu-26.04` under WSL 2.

- Install the packages listed in `doc/BUILD-win32.md` (Ubuntu section).
- Link `/opt/tools` to the clone's `contrib` folder (`sudo ln -s ~/src/vlx/contrib /opt/tools`): VideoLAN's prebuilt
  Qt looks for its native tools (qmake6, moc) there, as on their build machines.
  Its config gives that path relative to VideoLAN's folders, which resolves to `/home/opt/tools` in a clone at
  `~/src/vlx`, so link that too: `sudo mkdir -p /home/opt && sudo ln -s ~/src/vlx/contrib /home/opt/tools`.
  (Editing the config doesn't last: build.sh unpacks the prebuilt package again on every run.)
- Build from a Linux path (e.g. `~/src/vlx`, a clone of this repo), not `/mnt/l/...`. Builds on the Windows drive are many times slower.
- Bring commits into the WSL clone with `git fetch /mnt/l/personal-projects/vlx/vlc main && git checkout -f FETCH_HEAD`,
  not by copying files: the Windows checkout has CRLF line endings, which break the shell scripts.
- After the first full build, rebuild only what changed with `ninja -C win64-meson` (a minute or two), then copy
  `win64-meson/vlc-win64` to a Windows scratch folder to run it, e.g. `T:\dev\vlx\build-<HHmm>\`.

## 1. Every change

1. **Build** with no errors and no new warnings in the files you touched. Point the build at VideoLAN's prebuilt
   dependencies first, or it compiles all of them (Qt included) for hours:
   ```sh
   export VLC_PREBUILT_CONTRIBS_URL=https://artifacts.videolan.org/vlc/win64/vlc-contrib-x86_64-w64-mingw32-$(cat .vlx-contrib-sha).tar.zst
   ./extras/package/win32/build.sh -a x86_64 -p -m          # debug
   ./extras/package/win32/build.sh -a x86_64 -p -m -r -t    # release + LTO, what VLX ships
   ```
2. **Tests:** `meson test --suite qt` from the build directory (Qt models and QML tests). Run all of `meson test` before taking an upstream update.
3. **Start it in a scratch profile.** VLC uses a `portable` folder next to `vlc.exe` as its whole profile when one
   exists (`src/win32/dirs.c`). So run from the scratch copy with an empty `portable\` folder beside the exe:
   ```powershell
   New-Item -ItemType Directory T:\dev\vlx\build-<HHmm>\portable
   T:\dev\vlx\build-<HHmm>\vlc.exe --verbose=2 --file-logging --logfile=T:\dev\vlx\build-<HHmm>\vlx.log
   ```
   It must reach the main window, and the log must have no QML errors or binding loops. Never run a test build
   against the owner's real profile (`%APPDATA%\vlc`).

## 2. UI changes: screenshots

*Planned, built with the first UI change (same idea as tTorrent's `TTORRENT_UISHOT_DIR`):* with `VLX_UISHOT_DIR`
set, VLX renders each main screen off-screen, 1.5 s apart, saves a PNG of each and quits. Nothing on the owner's
screen is captured.

```powershell
$env:VLX_UISHOT_DIR = "<empty folder>"
T:\dev\vlx\build-<HHmm>\vlc.exe <sample video> <sample audio>
Remove-Item Env:VLX_UISHOT_DIR
```

Screens: library home, video grid and table, music, browse; player with a video (controls shown); player with an
audio file (the audio view); playlist open; settings first page, a searched setting, Advanced; the main menus;
About; mini player.

Look at every screen the change touches in a dark and a light theme (and two accents), and for spacing changes in
each density. Check for:

- clipped text;
- borders colliding;
- missing hover and focus states;
- the system title bar showing anywhere;
- horizontal scrolling;
- colours that ignore the theme or the accent.

**Sample media:** open films only (Big Buck Bunny, Sintel, Tears of Steel) and Creative Commons music, kept in
`T:\dev\vlx\samples\`. Never the owner's files.

For a visible change, show the owner a before/after image.

## 3. Feature checks

Each feature gets a real-run check when it's built. Run the ones your change touches.

| Feature | Check |
|---|---|
| Theme and accent | Each preset theme × 3 accents in the screenshot pass. A custom accent that fails 3:1 on the background is adjusted (shown in Settings). |
| Settings search | Search `gpu`, `subs`, `dark`, `buffer`: each jumps to the right setting and outlines it. Esc closes the results. |
| Audio view | Play an MP3 with cover art and a FLAC without: art or a generated fallback, title, artist, album, the queue. |
| Control island | Hides after the timeout in fullscreen, comes back on mouse move, never covers subtitles. |
| Feature cuts | `extras/vlx/vlx.ini` build: the cut modules are absent from `plugins\`, and their menu entries are hidden. |
| Keyboard and mouse | After any player or library change: walk the mouse table in `extras/vlx/input.md` by hand and try the common keys (Space, f, Esc, arrows with Shift/Alt/Ctrl, Ctrl+Up/Down, m, n, p, e, t, [ ] = + -) in every bottom bar style. |
| Upstream update | Regenerate `extras/vlx/hotkeys.md` and diff it, then run the full `meson test` and the screenshot pass, and play the sample set (H.264, HEVC 10-bit, AV1, VP9, FLAC, Opus, a subtitle file). |

## 4. Benchmarks

Any claim that VLX is lighter or faster needs numbers, before and after, from the same machine and the same files.

- **Start-up:** launch to first UI frame, from the log. Median of 5 cold and 5 warm runs, in a scratch profile.
- **Memory:** private working set at idle in the library, and while playing the 4K HEVC sample for 10 minutes.
- **Playback:** CPU and GPU use, and dropped frames (Tools > Media Information > Statistics), on the 4K HEVC sample.
- **Install size:** size of the release folder and the `plugins\` count.

Report one table with each build's median and peak, plus the machine, date and build each column came from.
Single runs vary, so call small differences "within noise" unless they repeat.

## 5. Releases

Only when the owner asks. Windows x64 only: VLX is for the owner's Windows PC.

1. Run **Actions › Release (Windows) › Run workflow** first. It builds the setup `.exe` and the portable `.7z` in
   VideoLAN's build image and attaches them to the run as an artifact. Install it on Windows and run §4 on it.
2. When it's good, bump the version and push a tag: `git tag v<version> && git push origin v<version>`. The same
   workflow builds again and publishes a pre-release named `VLX <version>` with the setup, the portable `.7z` and
   `sha256-<version>.txt`. Notes credit VLC and name the upstream base from `.vlx-base`.
3. After an upstream update, bump the Docker image in `.github/workflows/release-windows.yml` to match
   `VLC_WIN64_IMAGE` in `extras/ci/gitlab-ci.yml`.

The files aren't code-signed, so SmartScreen may warn on first run.
