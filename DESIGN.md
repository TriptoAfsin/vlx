# Design: VLZ

How VLZ looks and behaves. New UI follows this file; if the code and this file disagree, fix one of them on purpose in
the same change. Locked 21 November 2026 from the owner's choices:

- Direction **H. Lean** of the mock-ups: https://claude.ai/artifact/WHctzM6vgwGQVqHduTJ4G3
- Player options and defaults: https://claude.ai/artifact/8JH39dt9NduZ6yGzr9sMNK
- Icon **Ripple, echo**: [`extras/vlz/brand/`](extras/vlz/brand/)

Source of truth in code (planned): `modules/gui/qt/vlz/` (theme tables, accent, density, player layouts, settings
page). Upstream files only get hooks marked `VLZ:` (see [`UPSTREAM.md`](UPSTREAM.md)).

## Principles

- **Lean and clean.** Flat surfaces, hairline rules, one accent. Nothing fancy: no blur, no gradients on chrome, no idle redraws.
- **The media is the loudest thing.** Chrome is quiet greys. Colour marks the accent (progress, selection, primary action) and states only.
- **Familiar map.** Top bar with tabs, player, library, settings. VLC users find everything where they expect it.
- **Yours to arrange.** Theme, accent, size, bar style, positions and buttons are all user settings, and **every UI setting shows a live preview** of its effect before you leave the page.
- **Motion is state.** Things fade and slide when they change state. Nothing decorates.
- **Plain copy.** Sentence case, short sentences, no tracked capitals.
- **Nothing VLC users rely on is lost.** Every keyboard shortcut and mouse behaviour VLC has keeps working with the same default, in every layout option. The list is [`extras/vlz/input.md`](extras/vlz/input.md).

## Colour

Themes: **Dark, Light, Paper, Ink, Nord, Mocha, Tokyo**, plus Follow system. Each theme is a token table:

| Token | Use |
|---|---|
| `bg` | Window and video surround |
| `panel` | Top bar, docked bars, sidebars, queue, island |
| `raised` | Hover, selected tab, chips, inputs |
| `line` | Hairline rules and borders |
| `text`, `muted` | Primary and secondary text |
| `accent` | Progress, selection bar, primary button, focus, toggles |
| `ok`, `warn`, `bad` | States (errors, warnings) |

| Theme | bg | panel | raised | line | text | muted | own accent |
|---|---|---|---|---|---|---|---|
| Dark | `#141415` | `#111112` | `#1E1E21` | `#27272B` | `#ECECEE` | `#A1A1A7` | `#FF8800` |
| Light | `#FFFFFF` | `#FAFAFB` | `#F0F0F3` | `#E8E8EC` | `#18181B` | `#55555D` | `#D96A00` |
| Paper | `#F4EEE3` | `#EFE8DB` | `#E8DFCF` | `#DCD1BF` | `#2B241D` | `#5F5346` | `#A65424` |
| Ink | `#000000` | `#050506` | `#131316` | `#1E1E23` | `#F4F4F6` | `#A7A7B0` | `#8EA8FF` |
| Nord | `#2E3440` | `#2A303B` | `#3B4252` | `#434C5E` | `#ECEFF4` | `#C3CAD6` | `#88C0D0` |
| Mocha | `#1E1E2E` | `#181825` | `#313244` | `#313244` | `#CDD6F4` | `#BAC2DE` | `#CBA6F7` |
| Tokyo | `#1A1B26` | `#16161E` | `#24283B` | `#292E42` | `#C0CAF5` | `#A9B1D6` | `#7AA2F7` |

**Accent:** Theme (the theme's own), VLC orange `#FF8800`, Amber `#E6B450`, Rose `#E0466A`, Blue `#5B8CFF`,
Emerald `#10B981`, Grape `#8B5CF6`, Wine `#9F234A`, Mono (the text colour), or any colour. The chosen accent is
mixed toward white (dark themes) or black (light themes) until it has **3:1 contrast on `bg`**; Settings says when
it was adjusted. Derived: `select` = accent over `bg` (14 % light, 22 % dark), `track` = text at 16 %,
`onAccent` = `bg` or `text`, whichever contrasts more.

Video-overlaid text (titles, subtitles over the picture) uses a fixed warm white on a dark scrim, whatever the theme.

## Type

| Role | Face | Size at 100 % |
|---|---|---|
| UI | Segoe UI Variable (Windows' own; system font elsewhere) | 13 px |
| Numbers | Same face, tabular figures | UI size: times, sizes, codecs, counts |
| Titles (player, library card) | Segoe UI Variable Display semibold | 20 to 32 px |
| Settings group titles | Semibold | 15 px |

Scale ratio about 1.2. No bundled fonts by default (tTorrent found bundled faces looked choppy on Windows).

## Size, density, shape

- **Interface size:** 80, 90, 100, 110, 125, 150 % (VLC's existing `qt-interface-scale`), applied at once, with a preview.
- **Density:** Dense, Compact (default), Standard: control padding 3 / 4 / 7 px, row padding 3 / 4 / 7 px, layout spacing 4 / 6 / 8 px.
- **Radius:** controls 8 px, cards and panels 12 px, the island 16 px, pills fully round. One radius per nesting level.
- **Spacing:** 4 px grid. Controls 32 to 40 px tall, 44 px hit targets in the player.

## Motion

| Token | Value | Use |
|---|---|---|
| `micro` | 120 ms | Hover, press, toggles |
| `short` | 180 ms | View cross-fades, menus, search results |
| `medium` | 220 ms | Bars and island showing or hiding (fade plus a 6 px slide) |
| easing | out-cubic in, in-cubic out | Everything; no springs, no bounces |

Animate opacity and position only, never width or height (the current sidebar and playlist width animations go).
Reduced motion: everything changes instantly.

## Player

Every option below is a setting (Settings › Player, and right-click on the bar), each with a live preview.
Defaults are the owner's picks.

| Setting | Options | Default |
|---|---|---|
| Bottom bar style | **Island** (floating panel), **Docked** (full-width bar, seek line on its top edge), **Minimal** (no panel: scrim, thin seek line, white icons), **Compact pill** (play, seek, time, fullscreen, the rest behind ···), **Classic** (VLC's two rows, flat) | Classic |
| Controls position | Bottom, Top | Bottom |
| Playlist | Hidden, Side panel (video shrinks), Overlay (floats on the right, hides with the controls), Thumbnail shelf (above the bar) | Overlay |
| Top bar | With the controls, Always shown (windowed), Hidden (window buttons on corner hover) | With the controls |
| Top bar placement | Over the video, Above the video (its own row) | Over the video |
| Top bar items | Back to library, Title, Chapter name, File info, Clock, Ends at, Cast, Window buttons | Title, Chapter name, Window buttons |
| Bottom bar buttons (play, previous, next always) | Skip 10 s, Speed, Subtitles, Audio track, Playlist, Volume, Picture in picture, Fullscreen, Screenshot, Loop, Shuffle, Equaliser | Skip, Speed, Subtitles, Audio track, Playlist, Volume, PiP, Fullscreen, Screenshot |
| Time display | Elapsed and remaining, Elapsed and total, Elapsed and ends at | Elapsed and remaining |
| Seek bar | Chapter marks, Thumbnail on hover, Thicker on hover | Chapter marks, Thumbnail on hover |
| Title bar buttons | Play queue button and menu button (⋮), each Shown or Hidden. Hidden, the menu still opens with a right-click on the video, and Ctrl+K finds every action | Both shown |
| Bar background (overlay styles) | Fade over the video (VLC's), Stronger fade (dark from a third of the way up, 90 %), Solid bar | Fade over the video |
| Controls alignment (where play, previous and next sit) | Centered, Left, Right | Centered |
| Bar size | Compact, Normal, Large | Normal |
| Always show the controls (under the video, like VLC 3) | Off (over the video, auto-hide), On | Off |
| Always show the top bar (above the video) | Off (over the video, auto-hide), On | Off |
| When playback ends | Stay on the player (VLZ icon and the last title, like VLC 3), Go back to Home (VLC 4) | Stay on the player |
| Hide controls after | 1, 2, 3, 5 s, Never | 2 s |
| Opening files (from Explorer while VLZ runs) | A new window for each file, Play in the open window, Queue in the open window | Play in the open window (VLC's default) |
| Player buttons | VLC 4's toolbar editor: buttons of the video, audio and mini player bars, with profiles | VLC's default layout |
| Look defaults | Theme, accent, interface size | Ink, Emerald, 100 % |

- Subtitles move up while the controls show, so the bar never covers them.
- In fullscreen only the bar (and top bar, if set) shows, and both hide on the timer. The mouse pointer hides with them.
- **Audio player:** see the Audio player section below.

## Library

Top bar tabs: Now playing, Library, Browse, Settings, plus search (Ctrl K) and window buttons (client-side decorations).
The library opens on one **resume card** (the last unfinished item: art, title, time left, Resume and Start over),
then a **sortable table**: thumbnail, title, length, video, state pill (New, In progress in `accent`, Watched).
A grid view stays available. Artwork stays small, so untagged folders still look right.

## Settings

- A sidebar of sections with 15 px icons: Interface, Player, Playback, Video, Audio, Subtitles, Input and codecs, Hotkeys, Library, Advanced.
- **Search** at the top, drawn as a field even before it has focus: a magnifier, the `raised` fill and a 1 px `line` border (2 px `accent` when focused). Results list the setting and its section; Up/Down and Enter, Esc closes. Choosing one scrolls to the setting and outlines it in `accent` for 1.4 s. It matches everyday words too ("gpu" finds Hardware decoding, "subs" finds subtitles, "buffer" finds Network caching).
- Groups are `panel` cards with the title above. Every look and layout setting shows its live preview at the top of its page (a mini player that redraws as you change theme, accent, size, bar style, positions and buttons).
- **Advanced › Show every VLC setting** opens VLC's full preferences tree, for the rare deep change.

- **Where you are:** the section list follows scrolling. The section at the top of the view gets a fill one step off the
  list (`text` at 9 % over it) and a 3 px `accent` bar on its left, the play queue's playing-row marker; the others are
  `muted`. At the end of the page, the last section.
- **Opening it:** the sidebar's gear, Tools › Settings and **Ctrl+,** from anywhere, the player included. Ctrl+P keeps
  opening VLC's full Preferences, which carry the same VLZ settings (Interface › Look (VLZ)), Performance included.

## Menus

- Every entry and submenu has an icon: monochrome line icons (Fluent UI System Icons, MIT, `pixmaps/menu/vlz/`) in the
  menu's text colour. VLC's coloured menu icons are gone from menus; the colour stays with the video and cover art.
- Checkable entries keep their check mark in the icon column instead of an icon, as Windows menus do. Lists of
  tracks, devices and modes stay text.
- One table in `vlz/menuicons.cpp` maps each entry (by its translated text) to its icon, so VLC's menu code only has
  one-line hooks. A new VLC entry without a mapping shows no icon until it gets one.

## Dialogs

- The widget dialogs (Preferences, About, Help, Media Information…) follow the VLZ theme, not Windows' light or dark
  setting: the theme's colours, its colour scheme (so the title bar matches) and the accent. "VLC default" leaves them
  to VLC and Windows.
- On the accent, text and marks are near-black or white, whichever reads better (Emerald: near-black, 7.5:1). Links are
  the accent lifted toward `text` until they reach 4.5:1 on the surface behind them.
- Help is VLZ's own: its shortcuts and settings, VLC's documentation for the engine, and VLZ issues on GitHub.

## Audio player

When a file has no video, the player becomes a music view with its own skins (chosen 21 November 2026,
https://claude.ai/artifact/VkbX65YvQVoABugZxe3sWz). Settings › Player › Audio skin switches them, with a preview,
and a skin button in the player cycles through them.

| Skin | What it shows |
|---|---|
| **Album compact** (default) | A small window: cover, title, artist, seek line and controls, with a short track list under them that grows or hides as the window is resized |
| **Album** | Large cover, title, artist and album, format chips (codec, bit depth, sample rate), the album or queue always on the right |
| **Turntable** | The cover is the label of a record that spins while playing; the arm lifts on pause; up next under the title |
| **Turntable compact** | The record spinning beside the title and controls in a small window |
| **Mini player** | A slim strip (cover, title, seek line, previous, play, next) that can stay on top (Ctrl M) |

| Setting | Options | Default |
|---|---|---|
| Background | Plain theme, Tinted from the cover art | Plain theme |
| Seek bar | Line, Waveform (not built yet) | Line |
| Colours | Follow the app theme, or its own theme (Dark, Light, Paper, Ink, Nord, Mocha, Tokyo) | Follow the app theme |

- Skin, background and colours are in Settings › Audio player and in the classic preferences (Interface › Look),
  each with a preview of the skin in its colours.

- Cover art comes from the file or folder; without one, a tile coloured from the file name.
- Spinning and the arm stop with Windows' "Animation effects" off. The record turns at one revolution per 3.2 s.
- Format chips: codec, bit depth, sample rate, channels, ReplayGain when present.
- The record stops redrawing while it is hidden or while the window is dragged (it holds its angle), so dragging
  stays smooth: frames where the window didn't follow the cursor went from 47 of 120 to 4 of 120.

## System integration

- **Windows media controls** (`qt-vlz-media-controls`, on by default): VLZ shows in the media flyout, the lock screen
  and on media keys with title, artist, album, cover and progress; play, pause, stop, previous and next work from
  there. Video shows its title; the controls are on only while something is playing or paused.
- **Closing while audio plays** fades the volume out over about 90 ms, stops, then quits, so no tail of sound is left
  in the device buffer (Bluetooth headsets held about a second of it). The volume is restored before quitting.
- The Windows volume mixer lists the app as VLZ.

## Performance

Settings › Performance. Measured on a 1080p H.264 clip (Task Manager's memory column, 2026-11-22):

| Setting | Effect | Default |
|---|---|---|
| **Lite mode** | One switch for the lightest VLZ (2026-11-24, private bytes, with the GDI font engine below): the interface drawn by Qt Quick's software renderer (video stays on the GPU), Memory saver, no media library, no background parsing or online metadata. Idle 119 → 42 MB, playing 294 → 217 MB (VLC 3.0.24: 236), UI ready 684 → 400 ms; next start. Overrides those settings at start-up without saving them, so turning it off restores them; their rows say so and are locked | Off |
| Memory saver | 2 decoder threads and 0.3 s read-ahead instead of VLC's automatic threads and 1 s: playing 209 → 173 MB | Off (CPU-only 4K decoding may stutter) |
| Media library | Off: no indexing, Home becomes the open-file page, idle 85 → 75 MB; next start | On |
| Font engine (`qt-vlz-font-engine`, advanced only) | GDI's font database instead of DirectWrite's, which maps every installed font file (394 files, 529 MB on the test PC) and an 85 MB shared cache at start-up. Both modes: idle −23 MB, working set −54 MB, start-up 35-80 ms sooner. Text, CJK, Arabic, Indic scripts and colour emoji look the same. `directwrite` goes back to Qt's default; next start | GDI |

- Floors: VLC 3.0.24 needs 124 MB to play the same clip with the same savings; VLZ's interface (Qt Quick) adds
  about 35 MB on top while playing. Under 100 MB while playing 1080p isn't reachable for a VLC-based player.
- Lite mode, as the software renderer draws it: no window backdrop is possible there, so the sidebar and bars use
  their no-backdrop colour, which VLZ lifts one step off the page (10 % white on dark themes, 4 % black on light)
  to match the backdrop's look; Windows with transparency effects off gets the same. Shader effects (soft
  shadows) aren't drawn. Everything else looks the same.
- Lite mode is in `src/vlz_lite.c` (its option table, with the reason for each entry) and one hook in `qt.cpp`
  for the renderer. Measured and left out because they saved nothing: no backdrop blur, no tray icon, no Lua
  extensions, the basic render loop, the QML interpreter. OpenGL instead of Direct3D 11 costs more (idle 203 MB).
- The play queue panel is built the first time it opens, not at start-up (a panel left open still opens at start).
- Decoder threads stay automatic in normal mode. Capping them only while the GPU decodes would save about 18 MB,
  but VLC fixes the thread count before it picks hardware decoding, so doing it without a glitch needs a restart
  inside the decoder module. Left out (2026-11-24) to keep the decoder untouched.

## File icons

Chosen 21 November 2026 (https://claude.ai/artifact/YBZzFeVgBZK83hguPHDNxF). Both styles ship; Settings ›
Interface › File icons switches them, with an Explorer-style preview.

| Setting | Options | Default |
|---|---|---|
| Style | **Page and band** (page, kind glyph, coloured band with the extension), **Echo** (page with the Ripple echo triangles tinted per kind) | Page and band |
| Look | Follow Windows, Light, Dark | Follow Windows |
| Colour | By file kind, All orange | By file kind |
| Extension label | Shown, Hidden (always dropped at 16 and 24 px) | Shown |

- Kinds and colours: video `#FF7A1A`, audio `#10B981`, playlist `#4F7CFF`, subtitles `#8B5CF6`, disc image `#64748B`.
- Light look: white page. Dark look: charcoal page (Page and band) or near-black page (Echo), with a lighter edge so it holds on dark Explorer.
- **Follow Windows:** Windows has no theme-aware icons for desktop-app file types, so VLZ re-registers each type's
  `DefaultIcon` (light or dark `.ico`) when Windows switches theme while VLZ runs (`WM_SETTINGCHANGE`,
  `ImmersiveColorSet`), checks again at start-up, then calls `SHChangeNotify(SHCNE_ASSOCCHANGED)`.
- Files: one multi-size `.ico` (16 to 256 px) per kind, style and look, generated from SVG into `extras/vlz/brand/filetypes/` (planned).

## Components

- **Buttons:** primary is accent-filled with `onAccent` text; secondary is `raised` with a `line` border; icon buttons are transparent until hovered. Every control has hover, pressed, focus (2 px accent ring) and disabled states.
- **Island:** `panel` at 94 %, 1 px `line`, 16 px radius, soft shadow, no blur.
- **Queue rows:** 16:10 thumbnail, title, mono-free tabular duration; the playing row gets `raised` fill and a 2 px accent bar on its left.
- **Toggles:** 24 × 14 px track, accent when on. **Chips and pills:** `raised`, fully round.
- **Menus and tooltips:** `raised`, 1 px `line`, 10 px radius.

## Do and don't

- Do take every colour from a token, and every size from `VLCStyle` / VLZ tokens.
- Do keep labels in sentence case, body face, a step smaller. Never tracked capitals.
- Don't add blur, glass or gradients to chrome. The video and cover art are the only colourful things.
- Don't hide a feature without a setting to bring it back (except the build-time cuts in `extras/vlz/vlz.ini`).
- Don't change upstream layouts in place. Put VLZ components in `modules/gui/qt/vlz/` and hook them in.
