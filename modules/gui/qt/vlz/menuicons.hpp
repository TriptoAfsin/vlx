/*****************************************************************************
 * menuicons.hpp: one monochrome icon for every menu entry (DESIGN.md › Menus)
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
#ifndef VLZ_MENUICONS_HPP
#define VLZ_MENUICONS_HPP

class QMenu;

namespace vlz {

/* Gives every entry and submenu of `menu` its VLZ icon (pixmaps/menu/vlz), tinted to the menu's text colour,
 * replacing VLC's coloured ones. Entries are matched by their translated text, the same string VLC's menu code
 * uses, so it works in every language. Checkable entries keep their check mark instead of an icon. Submenus are
 * decorated again each time they open, since VLC rebuilds several of them then. Unknown entries are left as they
 * are (track, device and mode lists, for example). */
void decorateMenu(QMenu *menu);

} // namespace vlz

#endif
