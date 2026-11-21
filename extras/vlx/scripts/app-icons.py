"""Copy the VLX icon over every VLC icon in share/icons (the plain, Christmas, April-fools and kb variants), so no
VLC cone ships anywhere. Re-run after an upstream merge brings icons back. Needs Pillow.

    python extras/vlx/scripts/app-icons.py
"""
import os
import shutil

from PIL import Image

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..")
BRAND = os.path.join(ROOT, "extras", "vlx", "brand")
ICONS = os.path.join(ROOT, "share", "icons")


def write_xpm(png, out, name):
    """A small XPM writer (Pillow reads XPM but can't write it): up to 90 colours, one char each, binary alpha."""
    img = Image.open(png).convert("RGBA")
    alpha = img.getchannel("A")
    pal = img.convert("RGB").quantize(colors=90, method=Image.Quantize.MEDIANCUT)
    colours = pal.getpalette()[: 90 * 3]
    chars = "".join(chr(c) for c in range(35, 127) if chr(c) not in '"\\')[:91]
    w, h = img.size
    used = sorted(set(pal.getdata()))
    key = {idx: chars[i + 1] for i, idx in enumerate(used)}
    decl = 'static char * %s_xpm[] = {' % name
    if os.path.exists(out):  # keep the array name C code includes it by (e.g. skins2's vlc_xpm)
        decl = next((l.strip() for l in open(out) if l.startswith("static")), decl)
    lines = ['/* XPM */', decl, '"%d %d %d 1",' % (w, h, len(used) + 1), '"%s\tc None",' % chars[0]]
    for idx in used:
        r, g, b = colours[idx * 3: idx * 3 + 3]
        lines.append('"%s\tc #%02X%02X%02X",' % (key[idx], r, g, b))
    px, a = list(pal.getdata()), list(alpha.getdata())
    rows = []
    for y in range(h):
        rows.append('"' + "".join(chars[0] if a[y * w + x] < 128 else key[px[y * w + x]] for x in range(w)) + '"')
    lines.append(",\n".join(rows) + "};")
    open(out, "w", newline="\n").write("\n".join(lines) + "\n")


for size in os.listdir(ICONS):
    folder = os.path.join(ICONS, size)
    if not os.path.isdir(folder):
        continue
    px = size.split("x")[0]
    src = os.path.join(BRAND, "vlx-icon-%s.png" % px)
    for f in os.listdir(folder):
        if not f.startswith("vlc"):
            continue
        dst = os.path.join(folder, f)
        if f.endswith(".png"):
            shutil.copyfile(src, dst)
        elif f.endswith(".xpm"):
            write_xpm(src, dst, os.path.splitext(f)[0].replace("-", "_"))
        print("VLX icon ->", os.path.relpath(dst, ROOT))
shutil.copyfile(os.path.join(BRAND, "vlx.ico"), os.path.join(ICONS, "vlc.ico"))
