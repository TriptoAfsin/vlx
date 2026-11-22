# Taking in VLC updates

VLZ is VLC with a new look and a lighter build. `main` sits on a VLC base, with VLZ's changes on top as ordinary commits.
The goal is low upkeep: once the UI and the cut-down build feel right, VLZ only moves when VLC does.

## Which VLC VLZ follows

| Phase | Base | When to update |
|---|---|---|
| **Now** (VLC 4.0 not released yet) | A pinned, known-good commit of upstream `master` (4.0-dev) | Only when you choose, e.g. every month or two, or for a fix you want |
| **After VLC 4.0.0 ships** | Release tags: `4.0.0`, `4.0.1` … | When VideoLAN tags a release |

VLC 3.0.x is not an option: its interface is the old QWidget one, so the VLZ redesign doesn't apply to it.

The current base is written in [`.vlz-base`](.vlz-base) (one line: the upstream commit or tag). [`.vlz-contrib-sha`](.vlz-contrib-sha) is the last upstream commit that changed the dependencies; builds use it to fetch VideoLAN's prebuilt dependencies (`VLC_PREBUILT_CONTRIBS_URL`) instead of compiling them for hours.

## One-time setup

```sh
git remote add upstream https://github.com/videolan/vlc.git   # already done in this clone
git fetch upstream --tags
```

## Taking an update

```sh
git fetch upstream --tags
git switch -c update/<new-base> main
git merge <new-base>              # a master commit now, a tag like 4.0.1 later
# resolve conflicts: `git grep -n "VLZ:"` lists every hook in upstream files
# build, then run VERIFY.md (screenshot pass + feature checks)
echo <new-base> > .vlz-base
git rev-list -1 <new-base> -- extras/tools contrib extras/ci extras/package/win32 > .vlz-contrib-sha
git commit -am "vlz: take upstream <new-base>"
git switch main && git merge --ff-only update/<new-base>
```

Merge, don't rebase. A merge keeps VLZ's history readable and resolves each conflict once.

## Keeping updates painless

- **New code goes in new files.** VLZ's QML (control island, settings search, audio view, theme presets) lives under
  `modules/gui/qt/vlz/`. Upstream files only get a hook: an import, a `Loader`, one line.
- **Mark every edit to an upstream file** with a `VLZ:` comment (`// VLZ:` in C++/QML, `# VLZ:` in meson), so
  `git grep -n "VLZ:"` lists them all when an update conflicts.
- **Look is data.** Palettes, accents and density are token tables in VLZ's own files. Upstream's
  `systempalette.cpp` gets one hook that reads them, so palette changes never conflict.
- **Cut features with build switches, never by deleting code.** Everything chosen in the feature-cut list becomes a
  `-D<option>=disabled` or a `disable_modules` entry in one VLZ build profile (`extras/vlz/vlz.ini`). The module just
  isn't built, so it never ships or loads, and upstream code stays untouched. Menu entries for cut features are hidden
  at run time from one VLZ file.
- **The rebrand is a script** (`extras/vlz/rebrand.py`, planned). It's safe to run again after an update and re-applies
  names and icons that a merge brought back. Binary names, config folders and `PACKAGE` stay `vlc`, so plugins and
  settings keep working.
- **Strings stay translatable** in Qt's `qtr()`, so VLC's translation updates keep working.
