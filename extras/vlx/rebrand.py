#!/usr/bin/env python3
"""VLX rebrand: user-facing names and icons. Safe to run again after every upstream merge.

    python extras/vlx/rebrand.py          # apply
    python extras/vlx/rebrand.py --check  # list what would change, exit 1 if anything would

Binary names (vlc.exe, libvlc), config folders, the PACKAGE constant, registry keys and file-type
ProgIDs stay "vlc" on purpose, so plugins, settings and associations keep working (UPSTREAM.md).
"""
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BRAND = ROOT / 'extras' / 'vlx' / 'brand'
CHECK = '--check' in sys.argv

# (file, old, new). Each old string must exist unless new is already there.
STRINGS = [
    # Window title suffix, tray, dialogs
    ('modules/gui/qt/qt.cpp', 'app.setApplicationDisplayName( qtr("VLC media player") );', 'app.setApplicationDisplayName( qtr("VLX") );'),
    ('modules/gui/qt/qt.cpp', 'add_bool( "qt-icon-change", true,', 'add_bool( "qt-icon-change", false,'),
    ('modules/gui/qt/qt.cpp', '"allowing you to control VLC media player "', '"allowing you to control VLX "'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', 'setToolTip( qtr( "VLC media player" ));', 'setToolTip( qtr( "VLX" ));'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', 'std::make_unique<VLCMenu>( qtr( "VLC media player"), m_intf );', 'std::make_unique<VLCMenu>( qtr( "VLX"), m_intf );'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', '        setToolTip( qtr( "VLC media player" ) );', '        setToolTip( qtr( "VLX" ) );'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', 'showMessage( qtr( "VLC media player" ), name, icon, 3000 );', 'showMessage( qtr( "VLX" ), name, icon, 3000 );'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', 'qtr( "&Hide VLC media player in taskbar" )', 'qtr( "&Hide VLX in taskbar" )'),
    ('modules/gui/qt/dialogs/systray/systray.cpp', 'qtr( "Sho&w VLC media player" )', 'qtr( "Sho&w VLX" )'),
    ('modules/gui/qt/dialogs/help/help.cpp', '" + qtr( "VLC media player" ) + "', '" + qtr( "VLX" ) + "'),
    ('modules/gui/qt/dialogs/preferences/preferences.cpp', 'reset your VLC media player preferences?', 'reset your VLX preferences?'),
    ('modules/gui/qt/dialogs/firstrun/firstrunwizard.cpp', '<i>VLC media player</i>', '<i>VLX</i>'),
    ('modules/gui/qt/dialogs/preferences/preferences.cpp', 'qtr( "VLC needs to be restarted for the reset', 'qtr( "VLX needs to be restarted for the reset'),
    # Start-up error and console
    ('bin/winvlc.c', 'TEXT("VLC media player could not start.', 'TEXT("VLX could not start.'),
    ('bin/winvlc.c', '                    TEXT("VLC media player"),', '                    TEXT("VLX"),'),
    ('src/config/help.c', 'SetConsoleTitle (TEXT("VLC media player version ") TEXT(PACKAGE_VERSION));', 'SetConsoleTitle (TEXT("VLX version ") TEXT(PACKAGE_VERSION));'),
    # Windows executable resources
    ('bin/vlc_win32_rc.rc.in', 'VALUE "CompanyName", "VideoLAN"', 'VALUE "CompanyName", "VLX"'),
    ('bin/vlc_win32_rc.rc.in', 'VALUE "ProductName", "VLC media player"', 'VALUE "ProductName", "VLX"'),
    ('bin/vlc_win32_rc.rc.in', 'VALUE "FileDescription", "VLC media player"', 'VALUE "FileDescription", "VLX media player"'),
    ('bin/vlc_win32_rc.rc.in', 'VALUE "LegalCopyright", "Copyright \\251 @COPYRIGHT_YEARS@ VideoLAN and VLC Authors"',
     'VALUE "LegalCopyright", "VLX contributors. Based on VLC, copyright \\251 @COPYRIGHT_YEARS@ VideoLAN and VLC Authors"'),
    ('bin/vlc_win32_rc.rc.in', 'VALUE "LegalTrademarks", "VLC media player, VideoLAN and x264 are registered trademarks from VideoLAN"',
     'VALUE "LegalTrademarks", "VLC, VideoLAN and x264 are trademarks of VideoLAN. VLX is not affiliated with VideoLAN."'),
    # Installer
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '!define PRODUCT_NAME "VLC media player"', '!define PRODUCT_NAME "VLX"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '!define PRODUCT_GROUP "VideoLAN"', '!define PRODUCT_GROUP "VLX"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '!define PRODUCT_PUBLISHER "VideoLAN"', '!define PRODUCT_PUBLISHER "VLX"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '!define PRODUCT_WEB_SITE "https://www.videolan.org/"', '!define PRODUCT_WEB_SITE "https://github.com/TriptoAfsin/vlx"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '!define PRODUCT_ID "{ea92ef52-afe4-4212-bacb-dfe9fca94cd6}"', '!define PRODUCT_ID "{6b1f3c2e-4d7a-4f6e-9a8b-5c2d1e0f7a91}"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', 'InstallDir "$@PROGRAMFILES@\\${PRODUCT_PUBLISHER}\\VLC"', 'InstallDir "$@PROGRAMFILES@\\${PRODUCT_NAME}"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', 'BrandingText "${PRODUCT_GROUP} ${PRODUCT_NAME}"', 'BrandingText "${PRODUCT_NAME}, based on VLC media player"'),
    ('extras/package/win32/NSIS/vlc.win32.nsi.in', '"ApplicationDescription" "VLC - The video swiss knife"', '"ApplicationDescription" "VLX, a modern media player based on VLC"'),
    ('extras/package/win32/NSIS/helpers/extensions.nsh', 'WriteRegStr HKCR "VLC$R0" "" "$R2 $R1 File (VLC)"', 'WriteRegStr HKCR "VLC$R0" "" "$R2 $R1 File (VLX)"'),
]

# (file, regex, replacement): for long strings. The replacement must not match the regex again.
PATTERNS = [
    ('modules/gui/qt/dialogs/help/help.cpp',
     r'qtr\( "<p>VLC media player is a free and open source media player.*?" \) \+',
     'qtr( "<p>VLX is a personal, open source fork of <a href=\\"https://www.videolan.org/vlc/\\">VLC media player</a>. '
     'All credit for the engine and codecs goes to the volunteers of the <a href=\\"https://www.videolan.org/\\">VideoLAN</a> community.</p>'
     '<p>VLX is not affiliated with VideoLAN.</p><p><a href=\\"https://github.com/TriptoAfsin/vlx\\">VLX on GitHub</a>" ) +'),
]

# (source in extras/vlx/brand, destination)
FILES = [
    ('vlx.ico', 'share/icons/vlc.ico'),
    ('vlx-icon-16.png', 'share/icons/16x16/vlc.png'),
    ('vlx-icon-32.png', 'share/icons/32x32/vlc.png'),
    ('vlx-icon-48.png', 'share/icons/48x48/vlc.png'),
    ('vlx-icon-128.png', 'share/icons/128x128/vlc.png'),
    ('vlx-icon-256.png', 'share/icons/256x256/vlc.png'),
    ('vlx-icon-512.png', 'share/vlc512x512.png'),
    ('vlx-icon.svg', 'modules/gui/qt/pixmaps/misc/cone.svg'),
]

changes = []
for rel, old, new in STRINGS:
    p = ROOT / rel
    raw = p.read_bytes()
    enc = 'utf-8'
    try:
        text = raw.decode(enc)
    except UnicodeDecodeError:
        enc = 'latin-1'
        text = raw.decode(enc)
    if old in text:
        changes.append(f'{rel}: {old[:60]}')
        if not CHECK:
            p.write_bytes(text.replace(old, new).encode(enc))
    elif new not in text:
        print(f'warning: {rel}: neither the VLC nor the VLX text was found; upstream changed it:\n  {old}', file=sys.stderr)

for rel, pat, rep in PATTERNS:
    p = ROOT / rel
    text = p.read_text(encoding='utf-8')
    new_text, n = re.subn(pat, lambda m: rep, text, count=1, flags=re.S)
    if n:
        changes.append(f'{rel}: /{pat[:50]}/')
        if not CHECK:
            p.write_text(new_text, encoding='utf-8')

for src, dst in FILES:
    s, d = BRAND / src, ROOT / dst
    if not d.exists() or d.read_bytes() != s.read_bytes():
        changes.append(f'{dst} <- {src}')
        if not CHECK:
            shutil.copyfile(s, d)

for c in changes:
    print(('would change ' if CHECK else 'changed ') + c)
print(f'{len(changes)} change(s).')
sys.exit(1 if CHECK and changes else 0)
