# VLC default hotkeys (Windows)

Generated from `src/libvlc-module.c` by `extras/vlz/scripts/list-hotkeys.py`. Every one of these must keep
working in VLZ, with the same default. Regenerate after taking an upstream update and diff it.

| Action | Default keys | Option |
|---|---|---|
| Fullscreen | `f` | `key-toggle-fullscreen` |
| Exit fullscreen | `Esc` | `key-leave-fullscreen` |
| Play/Pause | `Space` | `key-play-pause` |
| Pause only | `Browser Stop` | `key-pause` |
| Play only | `Browser Refresh` | `key-play` |
| Faster | `+` | `key-faster` |
| Slower | `-` | `key-slower` |
| Normal rate | `=` | `key-rate-normal` |
| Faster (fine) | `]` | `key-rate-faster-fine` |
| Slower (fine) | `[` | `key-rate-slower-fine` |
| Next | `n` | `key-next` |
| Previous | `p` | `key-prev` |
| Stop | `s` | `key-stop` |
| Position | `t` | `key-position` |
| Very short backwards jump | `Shift+Left` | `key-jump-extrashort` |
| Very short forward jump | `Shift+Right` | `key-jump+extrashort` |
| Short backwards jump | `Alt+Left` | `key-jump-short` |
| Short forward jump | `Alt+Right` | `key-jump+short` |
| Medium backwards jump | `Ctrl+Left` | `key-jump-medium` |
| Medium forward jump | `Ctrl+Right` | `key-jump+medium` |
| Long backwards jump | `Ctrl+Alt+Left` | `key-jump-long` |
| Long forward jump | `Ctrl+Alt+Right` | `key-jump+long` |
| Next frame | `e` | `key-frame-next` |
| Previous frame | `Shift+e` | `key-frame-previous` |
| Activate | `Enter` | `key-nav-activate` |
| Navigate up | `Up` | `key-nav-up` |
| Navigate down | `Down` | `key-nav-down` |
| Navigate left | `Left` | `key-nav-left` |
| Navigate right | `Right` | `key-nav-right` |
| Go to the DVD menu | `Shift+m` | `key-disc-menu` |
| Select previous DVD title | `Shift+o` | `key-title-prev` |
| Select next DVD title | `Shift+b` | `key-title-next` |
| Select prev DVD chapter | `Shift+p` | `key-chapter-prev` |
| Select next DVD chapter | `Shift+n` | `key-chapter-next` |
| Quit | `Ctrl+q` | `key-quit` |
| Volume up | `Ctrl+Up` | `key-vol-up` |
| Volume down | `Ctrl+Down` | `key-vol-down` |
| Mute | `m` | `key-vol-mute` |
| Subtitle delay up | `h` | `key-subdelay-up` |
| Subtitle delay down | `g` | `key-subdelay-down` |
| Subtitle sync / bookmark audio timestamp | `Shift+h` | `key-subsync-markaudio` |
| Subtitle sync / bookmark subtitle timestamp | `Shift+j` | `key-subsync-marksub` |
| Subtitle sync / synchronize audio & subtitle timestamps | `Shift+k` | `key-subsync-apply` |
| Subtitle sync / reset audio & subtitle synchronization | `Ctrl+Shift+k` | `key-subsync-reset` |
| Subtitle position up | none (assignable) | `key-subpos-up` |
| Subtitle position down | none (assignable) | `key-subpos-down` |
| Audio delay up | `k` | `key-audiodelay-up` |
| Audio delay down | `j` | `key-audiodelay-down` |
| Cycle audio track | `b` | `key-audio-track` |
| Cycle through audio devices | `Shift+a` | `key-audiodevice-cycle` |
| Cycle subtitle track in reverse order | `Alt+v` | `key-subtitle-revtrack` |
| Cycle subtitle track | `v` | `key-subtitle-track` |
| Toggle subtitles | `Shift+v` | `key-subtitle-toggle` |
| Toggle secondary subtitle control | `Ctrl+Shift+v` | `key-subtitle-control-secondary` |
| Cycle next program Service ID | `x` | `key-program-sid-next` |
| Cycle previous program Service ID | `Shift+x` | `key-program-sid-prev` |
| Cycle source aspect ratio | `a` | `key-aspect-ratio` |
| Cycle video crop | `c` | `key-crop` |
| Toggle autoscaling | `o` | `key-toggle-autoscale` |
| Increase scale factor | `Alt+o` | `key-incr-scalefactor` |
| Decrease scale factor | `Alt+Shift+o` | `key-decr-scalefactor` |
| Toggle deinterlacing | `d` | `key-deinterlace` |
| Cycle deinterlace modes | `Shift+d` | `key-deinterlace-mode` |
| Show controller in fullscreen | `i` | `key-intf-show` |
| Boss key | none (assignable) | `key-intf-boss` |
| Context menu | `Menu` | `key-intf-popup-menu` |
| Take video snapshot | `Shift+s` | `key-snapshot` |
| Record | `Shift+r` | `key-record` |
| Zoom | `z` | `key-zoom` |
| Un-Zoom | `Shift+z` | `key-unzoom` |
| Toggle wallpaper mode in video output | `w` | `key-wallpaper` |
| Crop one pixel from the top of the video | `Alt+r` | `key-crop-top` |
| Uncrop one pixel from the top of the video | `Alt+Shift+r` | `key-uncrop-top` |
| Crop one pixel from the left of the video | `Alt+d` | `key-crop-left` |
| Uncrop one pixel from the left of the video | `Alt+Shift+d` | `key-uncrop-left` |
| Crop one pixel from the bottom of the video | `Alt+c` | `key-crop-bottom` |
| Uncrop one pixel from the bottom of the video | `Alt+Shift+c` | `key-uncrop-bottom` |
| Crop one pixel from the right of the video | `Alt+f` | `key-crop-right` |
| Uncrop one pixel from the right of the video | `Alt+Shift+f` | `key-uncrop-right` |
| Random | `r` | `key-random` |
| Normal/Loop/Repeat | `l` | `key-loop` |
| Shrink the viewpoint field of view (360°) | `Page Up` | `key-viewpoint-fov-in` |
| Expand the viewpoint field of view (360°) | `Page Down` | `key-viewpoint-fov-out` |
| Roll the viewpoint clockwise (360°) | none (assignable) | `key-viewpoint-roll-clock` |
| Roll the viewpoint anti-clockwise (360°) | none (assignable) | `key-viewpoint-roll-anticlock` |
| 1:4 Quarter | `Alt+1` | `key-zoom-quarter` |
| 1:2 Half | `Alt+2` | `key-zoom-half` |
| 1:1 Original | `Alt+3` | `key-zoom-original` |
| 2:1 Double | `Alt+4` | `key-zoom-double` |
| Clear the play queue | `Ctrl+w` | `key-clear-playlist` |
| Reset subtitles text scale | `Ctrl+0` | `key-subtitle-text-scale-normal` |
| Scale down subtitles text | `Ctrl+Mouse Wheel Up` | `key-subtitle-text-scale-up` |
| Scale up subtitles text | `Ctrl+Mouse Wheel Down` | `key-subtitle-text-scale-down` |
| Set play queue bookmark 1 | `Ctrl+F1` | `key-set-bookmark1` |
| Set play queue bookmark 2 | `Ctrl+F2` | `key-set-bookmark2` |
| Set play queue bookmark 3 | `Ctrl+F3` | `key-set-bookmark3` |
| Set play queue bookmark 4 | `Ctrl+F4` | `key-set-bookmark4` |
| Set play queue bookmark 5 | `Ctrl+F5` | `key-set-bookmark5` |
| Set play queue bookmark 6 | `Ctrl+F6` | `key-set-bookmark6` |
| Set play queue bookmark 7 | `Ctrl+F7` | `key-set-bookmark7` |
| Set play queue bookmark 8 | `Ctrl+F8` | `key-set-bookmark8` |
| Set play queue bookmark 9 | `Ctrl+F9` | `key-set-bookmark9` |
| Set play queue bookmark 10 | `Ctrl+F10` | `key-set-bookmark10` |
| Play play queue bookmark 1 | `F1` | `key-play-bookmark1` |
| Play play queue bookmark 2 | `F2` | `key-play-bookmark2` |
| Play play queue bookmark 3 | `F3` | `key-play-bookmark3` |
| Play play queue bookmark 4 | `F4` | `key-play-bookmark4` |
| Play play queue bookmark 5 | `F5` | `key-play-bookmark5` |
| Play play queue bookmark 6 | `F6` | `key-play-bookmark6` |
| Play play queue bookmark 7 | `F7` | `key-play-bookmark7` |
| Play play queue bookmark 8 | `F8` | `key-play-bookmark8` |
| Play play queue bookmark 9 | `F9` | `key-play-bookmark9` |
| Play play queue bookmark 10 | `F10` | `key-play-bookmark10` |

113 actions.
