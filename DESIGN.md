# Design: VLX

How VLX looks and behaves. New UI follows this file; if the code and this file disagree, fix one of them on purpose in
the same change. Locked 21 November 2026 from the owner's choices:

- Direction **H. Lean** of the mock-ups: https://claude.ai/artifact/WHctzM6vgwGQVqHduTJ4G3
- Player options and defaults: https://claude.ai/artifact/8JH39dt9NduZ6yGzr9sMNK
- Icon **Ripple, echo**: [`extras/vlx/brand/`](extras/vlx/brand/)

Source of truth in code (planned): `modules/gui/qt/vlx/` (theme tables, accent, density, player layouts, settings
page). Upstream files only get hooks marked `VLX:` (see [`UPSTREAM.md`](UPSTREAM.md)).

## Principles

- **Lean and clean.** Flat surfaces, hairline rules, one accent. Nothing fancy: no blur, no gradients on chrome, no idle redraws.
- **The media is the loudest thing.** Chrome is quiet greys. Colour marks the accent (progress, selection, primary action) and states only.
- **Familiar map.** Top bar with tabs, player, library, settings. VLC users find everything where they expect it.
- **Yours to arrange.** Theme, accent, size, bar style, positions and buttons are all user settings, and **every UI setting shows a live preview** of its effect before you leave the page.
- **Motion is state.** Things fade and slide when they change state. Nothing decorates.
- **Plain copy.** Sentence case, short sentences, no tracked capitals.

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
| Hide controls after | 1, 2, 3, 5 s, Never | 2 s |
| Look defaults | Theme, accent, interface size | Ink, Emerald, 100 % |

- Subtitles move up while the controls show, so the bar never covers them.
- In fullscreen only the bar (and top bar, if set) shows, and both hide on the timer. The mouse pointer hides with them.
- **Audio view:** when a file has no video, the player shows cover art (or a tile coloured from the file name), title, artist and album, format chips (codec, bit depth, sample rate, ReplayGain), and a waveform seek bar. The album or queue sits on the right. The mini player (Ctrl M) keeps the art and the controls.

## Library

Top bar tabs: Now playing, Library, Browse, Settings, plus search (Ctrl K) and window buttons (client-side decorations).
The library opens on one **resume card** (the last unfinished item: art, title, time left, Resume and Start over),
then a **sortable table**: thumbnail, title, length, video, state pill (New, In progress in `accent`, Watched).
A grid view stays available. Artwork stays small, so untagged folders still look right.

## Settings

- A sidebar of sections with 15 px icons: Interface, Player, Playback, Video, Audio, Subtitles, Input and codecs, Hotkeys, Library, Advanced.
- **Search** at the top (Ctrl F): results list the setting and its section; Up/Down and Enter, Esc closes. Choosing one scrolls to the setting and outlines it in `accent` for 1.4 s. It matches everyday words too ("gpu" finds Hardware decoding, "subs" finds subtitles, "buffer" finds Network caching).
- Groups are `panel` cards with the title above. Every look and layout setting shows its live preview at the top of its page (a mini player that redraws as you change theme, accent, size, bar style, positions and buttons).
- **Advanced › Show every VLC setting** opens VLC's full preferences tree, for the rare deep change.

## Components

- **Buttons:** primary is accent-filled with `onAccent` text; secondary is `raised` with a `line` border; icon buttons are transparent until hovered. Every control has hover, pressed, focus (2 px accent ring) and disabled states.
- **Island:** `panel` at 94 %, 1 px `line`, 16 px radius, soft shadow, no blur.
- **Queue rows:** 16:10 thumbnail, title, mono-free tabular duration; the playing row gets `raised` fill and a 2 px accent bar on its left.
- **Toggles:** 24 × 14 px track, accent when on. **Chips and pills:** `raised`, fully round.
- **Menus and tooltips:** `raised`, 1 px `line`, 10 px radius.

## Do and don't

- Do take every colour from a token, and every size from `VLCStyle` / VLX tokens.
- Do keep labels in sentence case, body face, a step smaller. Never tracked capitals.
- Don't add blur, glass or gradients to chrome. The video and cover art are the only colourful things.
- Don't hide a feature without a setting to bring it back (except the build-time cuts in `extras/vlx/vlx.ini`).
- Don't change upstream layouts in place. Put VLX components in `modules/gui/qt/vlx/` and hook them in.
