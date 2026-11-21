/*****************************************************************************
 * fileicons_win32.cpp: VLX file-type icons that follow the style and theme
 *****************************************************************************
 * Copyright (C) 2026 VLX contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "fileicons_win32.hpp"

#include "qt.hpp"

#include <QApplication>
#include <QRegularExpression>
#include <QTimer>

#include <string>

#include <windows.h>
#include <shlobj.h>

namespace vlx {

namespace {

const wchar_t *const kAssociations = L"Software\\Clients\\Media\\VLX\\Capabilities\\FileAssociations";

bool appsUseLightTheme()
{
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS)
        return true;
    return value != 0;
}

bool readString(HKEY root, const std::wstring &key, QString &out)
{
    wchar_t buf[2 * MAX_PATH];
    DWORD size = sizeof(buf);
    if (RegGetValueW(root, key.c_str(), nullptr, RRF_RT_REG_SZ, nullptr, buf, &size) != ERROR_SUCCESS)
        return false;
    out = QString::fromWCharArray(buf);
    return true;
}

} // namespace

FileIconSync::FileIconSync(qt_intf_t *intf, QObject *parent)
    : QObject(parent)
    , m_intf(intf)
    , m_debounce(new QTimer(this))
{
    m_debounce->setSingleShot(true);
    m_debounce->setInterval(500);
    connect(m_debounce, &QTimer::timeout, this, &FileIconSync::apply);
    qApp->installNativeEventFilter(this);

    /* Not urgent: leave start-up alone and do it a few seconds later. Then check every
     * 2 s, so a change on the settings page applies quickly; apply() returns at once
     * when nothing changed (two config reads and one registry value). */
    QTimer::singleShot(3000, this, &FileIconSync::apply);
    auto *poll = new QTimer(this);
    poll->setInterval(2000);
    connect(poll, &QTimer::timeout, this, &FileIconSync::apply);
    QTimer::singleShot(3000, poll, qOverload<>(&QTimer::start));
}

FileIconSync::~FileIconSync()
{
    qApp->removeNativeEventFilter(this);
}

QString FileIconSync::wantedFolder() const
{
    char *style = var_InheritString(m_intf, StyleOption);
    char *look = var_InheritString(m_intf, LookOption);

    const QString s = (style && strcmp(style, "echo") == 0) ? QStringLiteral("echo") : QStringLiteral("band");
    QString l = look ? QString::fromUtf8(look) : QStringLiteral("auto");
    free(style);
    free(look);

    if (l != QLatin1String("light") && l != QLatin1String("dark"))
        l = appsUseLightTheme() ? QStringLiteral("light") : QStringLiteral("dark");
    return s + QLatin1Char('-') + l;
}

void FileIconSync::apply()
{
    const QString folder = wantedFolder();
    if (folder == m_applied)
        return;

    HKEY caps;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kAssociations, 0, KEY_READ, &caps) != ERROR_SUCCESS)
    {
        m_applied = folder; /* VLX isn't registered for any file type */
        return;
    }

    static const QRegularExpression folderRe(QStringLiteral(R"((\\filetypes\\)[a-z]+-[a-z]+(\\))"),
                                             QRegularExpression::CaseInsensitiveOption);
    bool changed = false;

    for (DWORD i = 0; ; ++i)
    {
        wchar_t ext[256], progid[256];
        DWORD extLen = ARRAY_SIZE(ext), progidSize = sizeof(progid), type = 0;
        const LONG r = RegEnumValueW(caps, i, ext, &extLen, nullptr, &type,
                                     reinterpret_cast<LPBYTE>(progid), &progidSize);
        if (r == ERROR_NO_MORE_ITEMS)
            break;
        if (r != ERROR_SUCCESS || type != REG_SZ)
            continue;

        const std::wstring key = std::wstring(L"Software\\Classes\\") + progid + L"\\DefaultIcon";

        /* The machine-wide icon the installer wrote. Types installed without
         * VLX icons (no "\filetypes\" in the path) are left alone. */
        QString machine;
        if (!readString(HKEY_LOCAL_MACHINE, key, machine) || !folderRe.match(machine).hasMatch())
            continue;

        QString wanted = machine;
        wanted.replace(folderRe, QStringLiteral("\\1") + folder + QStringLiteral("\\2"));

        QString current;
        const bool hasOverride = readString(HKEY_CURRENT_USER, key, current);

        if (wanted == machine)
        {
            /* The installer's default: drop our per-user override, if any. */
            if (hasOverride && RegDeleteKeyValueW(HKEY_CURRENT_USER, key.c_str(), nullptr) == ERROR_SUCCESS)
                changed = true;
        }
        else if (!hasOverride || current != wanted)
        {
            const std::wstring value = wanted.toStdWString();
            if (RegSetKeyValueW(HKEY_CURRENT_USER, key.c_str(), nullptr, REG_SZ, value.c_str(),
                                static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t))) == ERROR_SUCCESS)
                changed = true;
        }
    }
    RegCloseKey(caps);

    if (changed)
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    m_applied = folder;
}

bool FileIconSync::nativeEventFilter(const QByteArray &, void *message, qintptr *)
{
    const MSG *msg = static_cast<const MSG *>(message);
    if (msg->message == WM_SETTINGCHANGE && msg->lParam
        && lstrcmpW(reinterpret_cast<LPCWSTR>(msg->lParam), L"ImmersiveColorSet") == 0)
        m_debounce->start(); /* Windows sends this several times per switch */
    return false;
}

} // namespace vlx
