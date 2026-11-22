/*****************************************************************************
 * fileicons_win32.hpp: VLZ file-type icons that follow the style and theme
 *****************************************************************************
 * Copyright (C) 2026 VLZ contributors
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
#ifndef VLZ_FILEICONS_WIN32_HPP
#define VLZ_FILEICONS_WIN32_HPP

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QString>

struct qt_intf_t;
class QTimer;

namespace vlz {

/*
 * The installer registers each file type with a light "Page and band" icon from
 * <install dir>\filetypes\band-light\. Windows has no theme-aware icons for desktop
 * apps, so this re-points each type to the chosen style and look with per-user
 * overrides under HKCU\Software\Classes (no admin rights needed): at start-up and
 * whenever Windows switches between light and dark. See DESIGN.md › File icons.
 */
class FileIconSync : public QObject, public QAbstractNativeEventFilter
{
public:
    FileIconSync(qt_intf_t *intf, QObject *parent);
    ~FileIconSync() override;

    /* Re-register the icons for the current settings, if they changed. */
    void apply();

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    /* Option values, also used by the settings page. */
    static constexpr const char *StyleOption = "qt-vlz-file-icons";
    static constexpr const char *LookOption = "qt-vlz-file-icon-look";

private:
    QString wantedFolder() const;

    qt_intf_t *m_intf;
    QTimer *m_debounce;
    QString m_applied;
};

} // namespace vlz

#endif
