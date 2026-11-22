/*****************************************************************************
 * Copyright (C) 2020 VLC authors and VideoLAN
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
import QtQuick.Layouts


import VLC.MainInterface
import VLC.Style
import VLC.Widgets as Widgets
import VLC.PlayerControls
import VLC.Player

FocusScope {
    id: playerControlLayout

    // Properties

    property real spacing: VLCStyle.margin_normal // spacing between controls

    property real layoutSpacing: VLCStyle.margin_xxlarge // spacing between layouts (left, center, and right)

    property int identifier: -1

    // VLX: where the playback group (the editor's "center" zone) sits: "center", "left" or "right".
    //      Left: [playback][left zone ......][right zone]; right: [left zone][right zone ......][playback].
    readonly property string vlxAlign: VLXSettings.barAlign

    readonly property PlayerControlbarModel model: {
        if (!!MainCtx.controlbarProfileModel.currentModel)
            return MainCtx.controlbarProfileModel.currentModel.getModel(identifier)
        else
            return null
    }

    readonly property ColorContext colorContext: ColorContext {
        id: theme
        colorSet: ColorContext.Window
    }

    // Signals

    signal requestLockUnlockAutoHide(bool lock)
    signal menuOpened(var menu)
    ///incomming signal to request all menu and popup to close
    signal forceUnlock()

    // Settings

    implicitWidth: loaderLeftRight.active ? loaderLeftRight.implicitWidth
                                          : (loaderLeft.implicitWidth + loaderCenter.implicitWidth + loaderRight.implicitWidth)

    implicitHeight: loaderLeftRight.active ? loaderLeftRight.implicitHeight
                                           : Math.max(loaderLeft.implicitHeight, loaderCenter.implicitHeight, loaderRight.implicitHeight)

    // Events

    Component.onCompleted: console.assert(identifier >= 0)

    // Children

    Loader {
        id: loaderLeftRight

        anchors.fill: parent

        active: !loaderCenter.active &&
                playerControlLayout.model &&
                ((playerControlLayout.model.left && (playerControlLayout.model.left.count > 0)) ||
                (playerControlLayout.model.right && (playerControlLayout.model.right.count > 0)))

        focus: active

        // VLX: two groups positioned by the alignment instead of one row, so "Controls alignment" also works
        //      for profiles with an empty centre zone (their playback buttons are in the left zone):
        //      left = VLC's layout; center = the left zone in the middle; right = the right zone first, then
        //      the left (playback) zone at the right edge.
        sourceComponent: Item {
            id: twoZones

            focus: true

            implicitWidth: leftGroup.implicitWidth + rightGroup.implicitWidth + playerControlLayout.layoutSpacing
            implicitHeight: Math.max(leftGroup.implicitHeight, rightGroup.implicitHeight)

            RowLayout {
                id: leftGroup

                height: parent.height
                width: implicitWidth
                spacing: playerControlLayout.spacing

                x: {
                    const a = playerControlLayout.vlxAlign
                    if (a === "right")
                        return Math.max(rightGroup.width + playerControlLayout.layoutSpacing, twoZones.width - width)
                    if (a === "center")
                        return Math.max(0, Math.min((twoZones.width - width) / 2,
                                                    twoZones.width - rightGroup.width - playerControlLayout.layoutSpacing - width))
                    return 0
                }
                Behavior on x { enabled: twoZones.width > 0; NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }

                ControlRepeater {
                    id: leftRepeater
                    model: ControlListFilter {
                        sourceModel: playerControlLayout.model.left

                        player: Player
                        ctx: MainCtx
                    }

                    Navigation.parentItem: playerControlLayout
                    Navigation.rightAction: function() {
                        const item = rightRepeater.itemAt(0)
                        if (item)
                            item.forceActiveFocus(Qt.TabFocusReason)
                        else
                            return false
                    }

                    availableWidth: loaderLeftRight.width
                    availableHeight: loaderLeftRight.height

                    Component.onCompleted: {
                        requestLockUnlockAutoHide.connect(playerControlLayout.requestLockUnlockAutoHide)
                        menuOpened.connect(playerControlLayout.menuOpened)
                        playerControlLayout.forceUnlock.connect(leftRepeater.forceUnlock)
                    }
                }
            }

            RowLayout {
                id: rightGroup

                height: parent.height
                width: implicitWidth
                spacing: playerControlLayout.spacing

                x: playerControlLayout.vlxAlign === "right" ? 0 : twoZones.width - width
                Behavior on x { enabled: twoZones.width > 0; NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }

                ControlRepeater {
                    id: rightRepeater
                    model: ControlListFilter {
                        sourceModel: playerControlLayout.model.right

                        player: Player
                        ctx: MainCtx
                    }

                    Navigation.parentItem: playerControlLayout
                    Navigation.leftAction: function() {
                        const item = leftRepeater.itemAt(leftRepeater.count - 1)
                        if (item)
                            item.forceActiveFocus(Qt.BacktabFocusReason)
                        else
                            return false
                    }

                    availableWidth: loaderLeftRight.width
                    availableHeight: loaderLeftRight.height

                    Component.onCompleted: {
                        requestLockUnlockAutoHide.connect(playerControlLayout.requestLockUnlockAutoHide)
                        menuOpened.connect(playerControlLayout.menuOpened)
                        playerControlLayout.forceUnlock.connect(rightRepeater.forceUnlock)
                    }
                }
            }
        }
    }


    Loader {
        id: loaderLeft

        anchors {
            right: playerControlLayout.vlxAlign === "left" ? loaderRight.left
                 : playerControlLayout.vlxAlign === "right" ? undefined : loaderCenter.left // VLX
            left: playerControlLayout.vlxAlign === "left" ? loaderCenter.right : parent.left // VLX
            top: parent.top
            bottom: parent.bottom

            // Spacing for the filler item acts as padding
            leftMargin: playerControlLayout.vlxAlign === "left" ? layoutSpacing - spacing : 0 // VLX
            rightMargin: playerControlLayout.vlxAlign === "right" ? 0 : layoutSpacing - spacing
        }

        active: !!playerControlLayout.model?.left && (playerControlLayout.model.left.count > 0) &&
                !loaderLeftRight.active

        focus: active

        sourceComponent: ControlLayout {
            id: leftControlLayout

            model: ControlListFilter {
                sourceModel: playerControlLayout.model.left

                player: Player
                ctx: MainCtx
            }

            alignment: (Qt.AlignVCenter | Qt.AlignLeft)

            spacing: playerControlLayout.spacing

            focus: true

            altFocusAction: () => Navigation.defaultNavigationRight()

            Navigation.parentItem: playerControlLayout
            Navigation.rightItem: loaderCenter.item

            Component.onCompleted: {
                requestLockUnlockAutoHide.connect(playerControlLayout.requestLockUnlockAutoHide)
                menuOpened.connect(playerControlLayout.menuOpened)
                playerControlLayout.forceUnlock.connect(leftControlLayout.forceUnlock)
            }
        }
    }

    Loader {
        id: loaderCenter

        anchors {
            // VLX: centered (VLC's layout), or at the left or right edge
            horizontalCenter: playerControlLayout.vlxAlign === "center" ? parent.horizontalCenter : undefined
            left: playerControlLayout.vlxAlign === "left" ? parent.left : undefined
            right: playerControlLayout.vlxAlign === "right" ? parent.right : undefined
            top: parent.top
            bottom: parent.bottom
        }

        // TODO: "ControlListFilter"'s count......
        active: !!playerControlLayout.model && !!playerControlLayout.model.center && (playerControlLayout.model.center.count > 0)

        Binding on width {
            delayed: true
            when: loaderCenter._componentCompleted
            value: {
                const item = loaderCenter.item

                const minimumWidth = (item && item.Layout.minimumWidth > 0) ? item.Layout.minimumWidth : implicitWidth
                const maximumWidth = (item && item.Layout.maximumWidth > 0) ? item.Layout.maximumWidth : implicitWidth

                if ((loaderLeft.active && (loaderLeft.width > 0)) || (loaderRight.active && (loaderRight.width > 0))) {
                    return minimumWidth
                } else {
                    return Math.min(loaderCenter.parent.width, maximumWidth)
                }
            }
        }

        property bool _componentCompleted: false

        Component.onCompleted: {
            _componentCompleted = true
        }

        sourceComponent: ControlLayout {
            id: centerControlLayout

            model: ControlListFilter {
                sourceModel: playerControlLayout.model.center

                player: Player
                ctx: MainCtx
            }

            focus: true

            spacing: playerControlLayout.spacing

            altFocusAction: () => Navigation.defaultNavigationUp()

            Navigation.parentItem: playerControlLayout
            Navigation.leftItem: loaderLeft.item
            Navigation.rightItem: loaderRight.item

            Component.onCompleted: {
                requestLockUnlockAutoHide.connect(playerControlLayout.requestLockUnlockAutoHide)
                menuOpened.connect(playerControlLayout.menuOpened)
                playerControlLayout.forceUnlock.connect(centerControlLayout.forceUnlock)
            }
        }
    }

    Loader {
        id: loaderRight

        anchors {
            // VLX: after the centered group (VLC), at the right edge (left alignment), or right after the left
            //      zone (right alignment: every other button on the left, the playback group alone on the right)
            left: playerControlLayout.vlxAlign === "center" ? loaderCenter.right
                : playerControlLayout.vlxAlign === "right" ? loaderLeft.right : undefined
            right: playerControlLayout.vlxAlign === "right" ? undefined : parent.right
            top: parent.top
            bottom: parent.bottom

            // Spacing for the filler item acts as padding
            leftMargin: playerControlLayout.vlxAlign === "left" ? 0 : layoutSpacing - spacing
            rightMargin: 0
        }

        active: !!playerControlLayout.model && !!playerControlLayout.model.right && (playerControlLayout.model.right.count > 0) &&
                !loaderLeftRight.active

        sourceComponent: ControlLayout {
            id: rightControlLayout

            model: ControlListFilter {
                sourceModel: playerControlLayout.model.right

                player: Player
                ctx: MainCtx
            }

            alignment: (Qt.AlignVCenter | Qt.AlignRight)

            spacing: playerControlLayout.spacing

            focus: true

            altFocusAction: () => Navigation.defaultNavigationLeft()

            Navigation.parentItem: playerControlLayout
            Navigation.leftItem: loaderCenter.item

            onRequestLockUnlockAutoHide: (lock) => playerControlLayout.requestLockUnlockAutoHide(lock)

            onMenuOpened: (menu) => playerControlLayout.menuOpened(menu)

            Component.onCompleted:{
                playerControlLayout.forceUnlock.connect(rightControlLayout.forceUnlock)
            }
        }
    }
}
