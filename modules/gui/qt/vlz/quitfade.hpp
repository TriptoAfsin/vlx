/*****************************************************************************
 * quitfade.hpp: quit without a click or a stale tail of audio
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
#ifndef VLZ_QUITFADE_HPP
#define VLZ_QUITFADE_HPP

#include <functional>

struct qt_intf_t;

namespace vlz {

/* Quitting while something plays cut the sound off mid-waveform: a click, and the tail of the music left in the
 * output device's own buffer (long with Bluetooth), heard at the next start. This hides the window at once, fades
 * the volume out over 90 ms, lets the output play silence for 120 ms so the device buffer drains to quiet, stops
 * playback, restores the volume (Windows remembers it per app), then calls `quit`. Without playback it quits
 * straight away. */
void quitAfterFade(qt_intf_t *intf, std::function<void()> quit);

} // namespace vlz

#endif
