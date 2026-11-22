"""VLZ installer strings: VLZ instead of VLC in every language (context menu, reinstall, uninstall). Re-run after
an upstream merge:  python extras/vlz/scripts/installer-strings.py"""
import glob
import os
import re

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "package", "win32", "NSIS", "languages")
total = 0
for p in sorted(glob.glob(os.path.join(ROOT, "*.nsh"))):
    raw = open(p, "rb").read()
    enc = "utf-8-sig" if raw.startswith(b"\xef\xbb\xbf") else "utf-8"
    try:
        s = raw.decode(enc)
    except UnicodeDecodeError:
        enc = "cp1252"
        s = raw.decode(enc)
    out, n = [], 0
    for line in s.splitlines(keepends=True):
        if "VLC" in line and "ActiveX" not in line:
            new = line.replace("VideoLAN VLC media player", "VLZ").replace("VLC media player", "VLZ")
            new = re.sub(r"\bVLC\b", "VLZ", new)
            if new != line:
                n += 1
            line = new
        out.append(line)
    if n:
        open(p, "wb").write("".join(out).encode(enc))
        total += n
        print("%-28s %3d lines" % (os.path.basename(p), n))
print("total", total)
