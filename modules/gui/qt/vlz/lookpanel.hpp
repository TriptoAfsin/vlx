/*****************************************************************************
 * lookpanel.hpp: VLZ look settings inside the classic preferences dialog
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
#ifndef VLZ_LOOKPANEL_HPP
#define VLZ_LOOKPANEL_HPP

#include "qt.hpp"

#include <QColor>
#include <QString>

class QWidget;

namespace vlz {

/* Colours of a VLZ theme, the same table the app uses (theme.cpp), with the accent made readable on it. */
struct ThemeColors
{
    bool dark;
    QColor bg, panel, raised, line, text, muted, accent;
};
ThemeColors themeColors(const QString &theme, const QString &accentOption);

/* "Look (VLZ)": theme, accent, size, player bar, audio skin and file icons with a live preview.
 * Every change applies at once, like the VLZ Settings page. */
QWidget *createLookPanel(qt_intf_t *intf, QWidget *parent);

} // namespace vlz

#endif
