#!/usr/bin/env python3
"""List VLC's default hotkeys on Windows as a Markdown table.

Reads src/libvlc-module.c: the KEY_* defaults in the non-macOS block (taking the
_WIN32 branch of nested #ifdefs) and the add_key() declarations with their labels.
Run from the repo root:  python -I extras/vlx/scripts/list-hotkeys.py > extras/vlx/hotkeys.md
"""
import re
import sys

src = open('src/libvlc-module.c', encoding='utf-8').read().splitlines()

# 1. Defaults: the "#else /* Non Mac OS X */" block up to its matching #endif.
start = next(i for i, l in enumerate(src) if 'Non Mac OS X' in l)
defaults, depth, skip = {}, 0, False
for line in src[start + 1:]:
    s = line.strip()
    if s.startswith('#if'):
        depth += 1
        skip = False                      # nested: take the first branch (_WIN32)
        continue
    if s.startswith('#else'):
        if depth == 0:
            break
        skip = True
        continue
    if s.startswith('#endif'):
        if depth == 0:
            break
        depth -= 1
        skip = False
        continue
    if skip:
        continue
    m = re.match(r'#\s*define\s+(KEY_\w+)\s+(NULL|"(.*)")', s)
    if m:
        defaults[m.group(1)] = None if m.group(2) == 'NULL' else m.group(3)

# 2. Labels: #define FOO_TEXT N_("...")
text = '\n'.join(src)
labels = dict(re.findall(r'#define\s+(\w+)\s+N_\(\s*"([^"]*)"', text))

# 3. add_key("key-xxx", KEY_XXX, LABEL_TEXT, ...)
rows = []
for name, key, label in re.findall(r'add_key\(\s*"([^"]+)"\s*,\s*(\w+)\s*,\s*(\w+)', text):
    val = defaults.get(key)  # NULL or undefined: no default key
    keys = ', '.join(f'`{k}`' for k in val.split('\t')) if val else 'none (assignable)'
    rows.append((labels.get(label, label), keys, name))

sys.stdout.reconfigure(encoding="utf-8", newline="\n")
out = sys.stdout
out.write('# VLC default hotkeys (Windows)\n\n')
out.write('Generated from `src/libvlc-module.c` by `extras/vlx/scripts/list-hotkeys.py`. Every one of these must keep\n')
out.write('working in VLX, with the same default. Regenerate after taking an upstream update and diff it.\n\n')
out.write('| Action | Default keys | Option |\n|---|---|---|\n')
for label, keys, name in rows:
    out.write(f'| {label} | {keys} | `{name}` |\n')
out.write(f'\n{len(rows)} actions.\n')
