/*****************************************************************************
 * mediacontrols_win32.hpp: Windows media controls (System Media Transport Controls)
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
#ifndef VLZ_MEDIACONTROLS_WIN32_HPP
#define VLZ_MEDIACONTROLS_WIN32_HPP

#include <QObject>
#include <memory>

struct qt_intf_t;
class QWindow;

namespace vlz {

/* Puts VLZ in Windows' media controls: the media flyout (volume keys, Windows 11's quick settings), the lock screen
 * and the keyboard's media keys show what plays (title, artist, album, cover, progress) and control it (play, pause,
 * stop, previous, next). VLC 4 has no such integration. "qt-vlz-media-controls" turns it off. */
class MediaControlsWin32 : public QObject
{
public:
    MediaControlsWin32(qt_intf_t *intf, QWindow *window, QObject *parent = nullptr);
    ~MediaControlsWin32() override;

private:
    struct Private;
    std::unique_ptr<Private> d;
};

} // namespace vlz

#endif
