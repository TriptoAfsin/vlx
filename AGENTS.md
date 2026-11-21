# AGENTS.md

Guidance for coding agents (and humans) working on VLX, a personal fork of VLC.

## What this repo is

- Upstream VLC `master` (4.0-dev) plus VLX changes on `main`. `master` mirrors `upstream/master` and is never edited.
- The VLX goal, in order: **modern, fluid UI**, then **performance**, then (later) features.
- The design system is in [`DESIGN.md`](DESIGN.md). Check changes the way [`VERIFY.md`](VERIFY.md) says.

## Where things live

| Area | Path |
|---|---|
| Qt interface (QML + C++) | `modules/gui/qt/` |
| Design tokens: sizes, fonts, durations | `modules/gui/qt/style/VLCStyle.qml` |
| Colours and theming | `modules/gui/qt/style/colorcontext.*`, `systempalette.*`, `*themeprovider*` |
| Player view, controls bar | `modules/gui/qt/player/` |
| Playlist | `modules/gui/qt/playlist/` |
| Media library views | `modules/gui/qt/medialibrary/` |
| Main window, CSD, acrylic | `modules/gui/qt/maininterface/`, `style/AcrylicController.qml` |
| Icon font, pixmaps | `modules/gui/qt/style/vlcicons.*`, `modules/gui/qt/pixmaps/` |
| App icons, Windows resources | `share/icons/`, `extras/package/win32/` |
| Core | `src/`, `include/`, `lib/` |
| Build | `meson.build`, `meson_options.txt` (Meson), `configure.ac` (autotools) |

## Rules

1. **Keep diffs small and contained.** VLX rebases on upstream regularly. Prefer changing tokens in `VLCStyle.qml`
   and theme providers over rewriting components. Prefer new files over large edits to upstream files.
2. **Mark VLX changes** in upstream files with a short `// VLX:` comment so rebases are easy to resolve.
3. **Don't drop features or formats** for speed. Performance work changes defaults, build flags or lazy loading.
4. **Follow upstream style:** C/C++ as in the surrounding code (4-space indent, C11/C++17), QML as in the
   surrounding components. Respect `VLCStyle` tokens; no hard-coded colours or pixel sizes in components.
5. **Licensing:** VLX code is GPLv2+ (LGPLv2.1+ inside `lib/`, `src/`, `include/`). Keep existing copyright headers.
6. **Branding:** use "VLX" and the VLX icon in user-facing branding. Credit VLC; never imply VideoLAN endorsement.
7. **Upstreaming:** if a change is sent to VideoLAN, follow [`doc/VideoLAN_AI_Policy.md`](doc/VideoLAN_AI_Policy.md).
   A human submits it, AI help is disclosed in the merge request, and commit messages don't credit AI tools.

## Git

- Work on `main` or a feature branch off it. Never commit to `master`.
- Sync: `git fetch upstream && git rebase upstream/master`.
- Commit messages follow upstream style: `module: short summary` (e.g. `qt: soften control bar surfaces`).
