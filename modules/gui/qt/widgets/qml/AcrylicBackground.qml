/*****************************************************************************
 * Copyright (C) 2021 VLC authors and VideoLAN
 *
 * Authors: Prince Gupta <guptaprince8832@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * ( at your option ) any later version.
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

import QtQuick


import VLC.Widgets
import VLC.Style


ViewBlockingRectangle {
    id: root

    readonly property bool usingAcrylic: visible && enabled && AcrylicController.enabled

    property color tintColor: "gray"

    property color alternativeColor: tintColor

    readonly property color _actualTintColor: tintColor.alpha(0.7)

    // VLZ: with no backdrop at all (Lite mode's software renderer, Windows transparency effects off), the
    // fallback is the page colour itself, so the sidebar and bars melt into the page. Lift it one step instead,
    // close to what the backdrop shows: about 10 % white over a dark theme, 4 % black over a light one.
    readonly property color _vlzFlatColor: root.alternativeColor.tint(root.alternativeColor.hslLightness < 0.5
                                                                     ? Qt.rgba(1, 1, 1, 0.1)
                                                                     : Qt.rgba(0, 0, 0, 0.04))

    color: !AcrylicController.enabled ? root._vlzFlatColor // VLZ
         : root._actualTintColor.tint(Qt.alpha(root.alternativeColor,
                                               1 - (usingAcrylic ? AcrylicController.uiTransluency : 0)))
}
