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

    // VLZ: where the playback group (the editor's "center" zone) sits: "center", "left" or "right".
    //      Left: [playback][left zone ......][right zone]; right: [left zone][right zone ......][playback].
    readonly property string vlzAlign: VLZSettings.barAlign

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

        // VLZ: two groups positioned by the alignment instead of one row, so "Controls alignment" also works
        //      for profiles with an empty centre zone (their playback buttons are in the left zone):
        //      left = VLC's layout; center = the left zone in the middle; right = the right zone first, then
        //      the left (playback) zone at the right edge.
        sourceComponent: Item {
            id: twoZones

            focus: true

            implicitWidth: leftGroup.implicitWidth + rightGroup.implicitWidth + playerControlLayout.layoutSpacing
            implicitHeight: Math.max(leftGroup.implicitHeight, rightGroup.implicitHeight)

            // the room the left (playback) zone may use beside the right zone. What fits is decided against this
            // room, not against the group's own width, which shrinks as buttons hide (they'd never come back)
            readonly property real leftRoom: Math.max(0, width - rightGroup.width
                                                         - (rightGroup.width > 0 ? playerControlLayout.layoutSpacing : 0))

            RowLayout {
                id: leftGroup

                height: parent.height
                width: Math.min(implicitWidth, twoZones.leftRoom)
                spacing: playerControlLayout.spacing

                x: {
                    const a = playerControlLayout.vlzAlign
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

                    // VLZ: buttons that don't fit in the room left of the right zone hide from the end
                    // (the group's x keeps its right edge within this room for every alignment)
                    availableWidth: twoZones.leftRoom
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
                // the volume slider shrinks (down to the group's minimum) before any playback button hides
                width: Math.min(implicitWidth, Math.max(vlzMinimumWidth,
                                                        twoZones.width - leftGroup.implicitWidth - playerControlLayout.layoutSpacing),
                                twoZones.width)
                spacing: playerControlLayout.spacing

                // VLZ: the group's minimum, summed from its visible buttons (their Loaders set Layout.minimumWidth).
                // Not the RowLayout's own Layout.minimumWidth: unset, Qt computes it from the layout's item list,
                // which can still hold a button the repeater has just destroyed when the buttons change (a video
                // starting), and VLZ crashed.
                readonly property real vlzMinimumWidth: {
                    let size = 0
                    let shown = 0
                    for (let i = 0; i < rightRepeater.count; ++i) {
                        const loader = rightRepeater.itemAt(i)
                        if (!loader || !loader.visible)
                            continue
                        size += Math.max(0, loader.Layout.minimumWidth)
                        ++shown
                    }
                    return size + Math.max(0, shown - 1) * spacing
                }

                x: playerControlLayout.vlzAlign === "right" ? 0 : twoZones.width - width
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

                    availableWidth: rightGroup.width // VLZ
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
            right: playerControlLayout.vlzAlign === "left" ? loaderRight.left
                 : playerControlLayout.vlzAlign === "right" ? undefined : loaderCenter.left // VLZ
            left: playerControlLayout.vlzAlign === "left" ? loaderCenter.right : parent.left // VLZ
            top: parent.top
            bottom: parent.bottom

            // Spacing for the filler item acts as padding
            leftMargin: playerControlLayout.vlzAlign === "left" ? layoutSpacing - spacing : 0 // VLZ
            rightMargin: playerControlLayout.vlzAlign === "right" ? 0 : layoutSpacing - spacing
        }

        // VLZ: right alignment leaves this zone one anchor; stop it before the right zone and the playback group
        width: playerControlLayout.vlzAlign !== "right" ? undefined
             : Math.min(implicitWidth, Math.max(0, loaderCenter.x - layoutSpacing
                                                    - (loaderRight.active ? loaderRight.implicitWidth + layoutSpacing : 0)))

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
            // VLZ: centered (VLC's layout), or at the left or right edge
            horizontalCenter: playerControlLayout.vlzAlign === "center" ? parent.horizontalCenter : undefined
            left: playerControlLayout.vlzAlign === "left" ? parent.left : undefined
            right: playerControlLayout.vlzAlign === "right" ? parent.right : undefined
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
            // VLZ: after the centered group (VLC), at the right edge (left alignment), or right after the left
            //      zone (right alignment: every other button on the left, the playback group alone on the right)
            left: playerControlLayout.vlzAlign === "center" ? loaderCenter.right
                : playerControlLayout.vlzAlign === "right" ? loaderLeft.right : undefined
            right: playerControlLayout.vlzAlign === "right" ? undefined : parent.right
            top: parent.top
            bottom: parent.bottom

            // Spacing for the filler item acts as padding
            leftMargin: playerControlLayout.vlzAlign === "left" ? 0 : layoutSpacing - spacing
            rightMargin: 0
        }

        // VLZ: with left or right alignment this zone has one anchor; keep it clear of the playback group
        width: playerControlLayout.vlzAlign === "center" ? undefined
             : playerControlLayout.vlzAlign === "left"
               ? Math.min(implicitWidth, Math.max(0, parent.width - loaderCenter.width - layoutSpacing))
               : Math.min(implicitWidth, Math.max(0, loaderCenter.x - layoutSpacing - x))

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
