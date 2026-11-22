# AGENTS.md

Guidance for coding agents (and humans) working on VLZ, a personal fork of VLC.

## What this repo is

- Upstream VLC `master` (4.0-dev) plus VLZ changes on `main`. `master` mirrors `upstream/master` and is never edited.
- The VLZ goal, in order: **modern, fluid UI**, then **performance**, then (later) features.
- The design system is in [`DESIGN.md`](DESIGN.md). Check changes the way [`VERIFY.md`](VERIFY.md) says. Take upstream VLC the way [`UPSTREAM.md`](UPSTREAM.md) says.
- Feature cuts live in [`extras/vlz/vlz.ini`](extras/vlz/vlz.ini) (Meson options). Never delete upstream code to cut a feature.
- Every UI setting (theme, accent, size, bar style, positions) shows a live preview of its effect in Settings.
- Every VLC keyboard shortcut and mouse behaviour must keep working with the same default: [`extras/vlz/input.md`](extras/vlz/input.md).

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

1. **Keep diffs small and contained.** VLZ takes in new VLC versions by merge. Prefer changing tokens in `VLCStyle.qml`
   and theme providers over rewriting components. Prefer new files over large edits to upstream files.
2. **Mark VLZ changes** in upstream files with a short `// VLZ:` comment so upstream merges are easy to resolve (`git grep -n "VLZ:"`).
3. **Cut features only through `extras/vlz/vlz.ini`**, and only the ones the owner picked. Never delete upstream code.
4. **Follow upstream style:** C/C++ as in the surrounding code (4-space indent, C11/C++17), QML as in the
   surrounding components. Respect `VLCStyle` tokens; no hard-coded colours or pixel sizes in components.
5. **Licensing:** VLZ code is GPLv2+ (LGPLv2.1+ inside `lib/`, `src/`, `include/`). Keep existing copyright headers.
6. **Branding:** use "VLZ" and the VLZ icon in user-facing branding. Credit VLC; never imply VideoLAN endorsement.
7. **Upstreaming:** if a change is sent to VideoLAN, follow [`doc/VideoLAN_AI_Policy.md`](doc/VideoLAN_AI_Policy.md).
   A human submits it, AI help is disclosed in the merge request, and commit messages don't credit AI tools.

## Git

- Work on `main` or a feature branch off it. Never commit to `master`.
- Taking upstream VLC: merge, never rebase. Steps in [`UPSTREAM.md`](UPSTREAM.md).
- Commit messages follow upstream style: `module: short summary` (e.g. `qt: soften control bar surfaces`).
