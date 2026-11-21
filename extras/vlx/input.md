# Keyboard and mouse: what VLX must keep

Rule: **every keyboard shortcut and mouse behaviour VLC has keeps working in VLX, with the same default.** A new
VLX layout (island, docked, minimal, pill, classic; any playlist mode) wires the same actions to its own controls.
Users can still remap keys in Settings › Hotkeys.

- Keyboard: [`hotkeys.md`](hotkeys.md), generated from `src/libvlc-module.c` (113 actions). Regenerate it after
  every upstream update (`python -I extras/vlx/scripts/list-hotkeys.py > extras/vlx/hotkeys.md`) and read the diff.
- Mouse: the table below, gathered from VLC's code at the fork point. Check each one by hand after a UI change.

## Mouse

| Where | Action | Result | Source |
|---|---|---|---|
| Video | Double-click (left) | Toggle fullscreen | `src/video_output/video_window.c` |
| Video | Right-click (on release, Windows) | Context menu | `modules/control/hotkeys.c` |
| Video | Left-click | Closes the context menu | `modules/control/hotkeys.c` |
| Video | Middle-click | Show or hide the fullscreen controls | `modules/control/hotkeys.c` |
| Video | Wheel up/down | Volume by default (`hotkeys-y-wheel-mode`: volume, position, none, reversed) | `src/libvlc-module.c`, `hotkeys.c` |
| Video | Wheel left/right (tilt) | Position by default (`hotkeys-x-wheel-mode`) | same |
| Video, 360° media | Left-drag | Look around (viewpoint) | `modules/control/hotkeys.c` |
| Video | Mouse still for `mouse-hide-timeout` | Pointer and controls hide; any move shows them | `src/libvlc-module.c` |
| Audio-only player background | Right-click | Audio context menu | `player/qml/PlayerView.qml` |
| Player area | Wheel | Same as on video (wheel mode setting) | `player/qml/PlayerView.qml` |
| Seek bar | Click, drag | Seek; hover shows the time | `player/qml/SliderBar.qml` |
| Seek bar | Wheel | Seek | `player/qml/SliderBar.qml` |
| Play button | Press and hold | Stop | `controlbarcontrols/PlayButton.qml` |
| Volume slider | Left-click or drag; right-click or right-drag | Set volume | `controlbarcontrols/VolumeWidget.qml` |
| Volume slider | Wheel (Shift for fine steps) | Volume up/down | `controlbarcontrols/VolumeWidget.qml` |
| Speed button | Wheel | Faster/slower | `controlbarcontrols/PlaybackSpeedButton.qml` |
| Speed button | Right-click | Reset to normal speed | `controlbarcontrols/PlaybackSpeedButton.qml` |
| Library | Mouse back button | Go back | `maininterface/qml/MainDisplay.qml` |
| Library, browse views | Double-click an item | Play or open it | `network/qml/BrowseDeviceView.qml`, list and grid views |
| Library, lists and tables | Right-click | Context menu | media library views |
| Window | Drop files or folders | Play them, or add them to the queue | `maininterface/qml/MainInterface.qml` |
| Playlist | Drag rows; drop files | Reorder; insert at the drop point | `playlist/qml/PlaylistPane.qml`, `PlaylistDelegate.qml` |
| Toolbar editor | Drag buttons | Customise the control bar | `dialogs/toolbar/qml/` |
| Title bar (client-side) | Drag, double-click, snap | Move, maximise, Windows snap layouts | `widgets/qml/CSDWindowButton*.qml` |

Kept features with their own mouse input: **Mouse gestures** (`gestures` module, kept in the feature cuts) and
global hotkeys (`modules/control/globalhotkeys`).
