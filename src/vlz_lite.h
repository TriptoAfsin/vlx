/*****************************************************************************
 * vlz_lite.h: VLZ Lite mode (Settings › Performance › Lite mode)
 *****************************************************************************
 * Copyright (C) 2026 VLZ contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifndef VLZ_LITE_H
#define VLZ_LITE_H

/* Whether Lite mode ("qt-vlz-lite") is on. */
bool vlz_LiteEnabled(vlc_object_t *obj);

/* Applies Lite mode's options as variables on the libvlc object. Call once, after the config is loaded. */
void vlz_LiteApply(vlc_object_t *libvlc);

#endif
