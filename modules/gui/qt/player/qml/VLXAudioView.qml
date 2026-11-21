/*****************************************************************************
 * VLXAudioView.qml: VLX music view for files without video (DESIGN.md › Audio player)
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
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import VLC.MainInterface
import VLC.Player
import VLC.Playlist
import VLC.Style
import VLC.Util
import VLC.Widgets as Widgets

// The content area only: transport, seeking and every shortcut stay on VLC's control bar,
// which follows the chosen bar style. Skins: albumcompact, album, turntable, turntablecompact, mini.
FocusScope {
    id: root

    property string skin: VLXSettings.audioSkin
    property real topPadding: 0
    property real bottomPadding: 0

    readonly property bool playing: Player.playingState === Player.PLAYING_STATE_PLAYING
    // without a title tag, the file name without its extension (Player.name adds the artist)
    readonly property string titleText: {
        if (Player.title !== "")
            return Player.title
        const path = decodeURIComponent(Player.url.toString())
        const file = path.substring(path.lastIndexOf("/") + 1)
        const dot = file.lastIndexOf(".")
        return (dot > 0 ? file.substring(0, dot) : file) || Player.name
    }
    readonly property string byText: [Player.artist, Player.album].filter(s => s !== "").join(", ")
    readonly property string trackText: (playlistModel.count > 1)
        ? qsTr("Track %1 of %2").arg(playlistModel.currentIndex + 1).arg(playlistModel.count) : ""

    readonly property ColorContext colorContext: ColorContext {
        id: theme
        colorSet: ColorContext.View
    }

    PlaylistListModel {
        id: playlistModel
        playlist: MainPlaylistController.playlist
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bg.primary
    }

    // ---------- shared parts ----------

    // Cover art, or a tile coloured from the title when the file has none
    component Cover: Item {
        id: cover
        property real radius: VLCStyle.dp(12, VLCStyle.scale)
        property bool showNote: true // off on a record label, where the spindle sits in the middle
        // no cover art: a tile in one hue taken from the title, with a note so it reads as "no art"
        readonly property real hue: (root.hash(root.titleText) % 360) / 360
        readonly property color fallbackA: Qt.hsla(hue, 0.5, 0.42, 1)
        readonly property color fallbackB: Qt.hsla((hue + 0.07) % 1, 0.45, 0.2, 1)

        Rectangle {
            anchors.fill: parent
            radius: cover.radius
            visible: art.status !== Image.Ready
            gradient: Gradient {
                orientation: Gradient.Vertical
                GradientStop { position: 0.0; color: cover.fallbackA }
                GradientStop { position: 1.0; color: cover.fallbackB }
            }

            Text {
                anchors.centerIn: parent
                visible: cover.showNote
                text: VLCIcons.topbar_music
                font.family: VLCIcons.fontFamily
                font.pixelSize: Math.min(parent.width, parent.height) * 0.36
                color: Qt.rgba(1, 1, 1, 0.55)
            }
        }

        Widgets.ImageExt {
            id: art
            anchors.fill: parent
            source: Player.artwork
            radius: cover.radius
            fillMode: Image.PreserveAspectCrop
            sourceSize: Qt.size(width * Screen.devicePixelRatio, height * Screen.devicePixelRatio)
            asynchronous: true
        }
    }

    // A record with the cover as its label; spins while playing, the arm lifts on pause
    component Disc: Item {
        id: disc
        implicitWidth: VLCStyle.dp(280, VLCStyle.scale)
        implicitHeight: implicitWidth

        Item {
            id: platter
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                radius: width / 2
                color: Qt.rgba(0.07, 0.07, 0.08, 1)
                border.color: Qt.rgba(0.03, 0.03, 0.04, 1)
                border.width: VLCStyle.dp(4, VLCStyle.scale)
            }
            Repeater { // grooves
                model: 7
                Rectangle {
                    required property int index
                    anchors.centerIn: parent
                    width: platter.width * (0.94 - index * 0.07)
                    height: width
                    radius: width / 2
                    color: "transparent"
                    border.color: Qt.rgba(1, 1, 1, 0.045)
                    border.width: 1
                }
            }
            Cover {
                anchors.centerIn: parent
                width: platter.width * 0.38
                height: width
                radius: width / 2
                showNote: false
            }
            Rectangle {
                anchors.centerIn: parent
                width: platter.width * 0.03
                height: width
                radius: width / 2
                color: theme.bg.primary
            }

            RotationAnimator on rotation {
                from: 0
                to: 360
                duration: 3200
                loops: Animation.Infinite
                running: root.playing && root.visible
            }
        }

        Item { // tone arm
            width: disc.width * 0.34
            height: disc.height * 0.62
            anchors.right: disc.right
            anchors.rightMargin: -disc.width * 0.04
            anchors.top: disc.top
            transformOrigin: Item.TopRight
            rotation: root.playing ? 0 : -24
            Behavior on rotation { NumberAnimation { duration: 700; easing.type: VLCStyle.easing_enter } }

            Rectangle {
                width: VLCStyle.dp(16, VLCStyle.scale); height: width; radius: width / 2
                anchors.right: parent.right; anchors.top: parent.top
                color: theme.bg.secondary; border.color: theme.border; border.width: VLCStyle.border
            }
            Rectangle {
                width: VLCStyle.dp(4, VLCStyle.scale)
                height: parent.height * 0.9
                x: parent.width - width - VLCStyle.dp(6, VLCStyle.scale)
                y: VLCStyle.dp(8, VLCStyle.scale)
                radius: width / 2
                color: theme.fg.secondary
                transformOrigin: Item.Top
                rotation: 18
            }
        }
    }

    component Titles: ColumnLayout {
        property real titleSize: VLCStyle.fontSize_xxlarge
        spacing: VLCStyle.margin_xxsmall

        Widgets.CaptionLabel {
            text: root.trackText
            visible: text !== ""
            color: theme.accent
            font.weight: Font.DemiBold
        }
        Widgets.SubtitleLabel {
            Layout.fillWidth: true
            text: root.titleText
            font.pixelSize: parent.titleSize
            font.weight: Font.Bold
            wrapMode: Text.Wrap
            maximumLineCount: 2
            elide: Text.ElideRight
            color: theme.fg.primary
        }
        Widgets.ListLabel {
            Layout.fillWidth: true
            text: root.byText
            visible: text !== ""
            elide: Text.ElideRight
            color: theme.fg.secondary
        }
    }

    component TrackList: ListView {
        clip: true
        model: playlistModel
        boundsBehavior: Flickable.StopAtBounds
        delegate: Rectangle {
            required property int index
            required property string title
            required property var duration
            required property bool isCurrent

            width: ListView.view.width
            height: VLCStyle.dp(36, VLCStyle.scale)
            color: isCurrent ? theme.bg.highlight : (hover.hovered ? theme.bg.secondary : "transparent")
            Behavior on color { ColorAnimation { duration: VLCStyle.duration_short } }

            Rectangle { // current-track bar
                width: VLCStyle.dp(2, VLCStyle.scale); height: parent.height
                color: theme.accent; visible: isCurrent
            }
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: VLCStyle.margin_normal
                anchors.rightMargin: VLCStyle.margin_normal
                spacing: VLCStyle.margin_small
                Widgets.CaptionLabel { text: index + 1; color: isCurrent ? theme.accent : theme.fg.secondary; Layout.preferredWidth: VLCStyle.dp(24, VLCStyle.scale) }
                Widgets.ListLabel { text: title; elide: Text.ElideRight; Layout.fillWidth: true; color: theme.fg.primary; font.weight: isCurrent ? Font.DemiBold : Font.Normal }
                Widgets.CaptionLabel { text: duration.formatHMS(); color: theme.fg.secondary }
            }
            HoverHandler { id: hover }
            TapHandler { onDoubleTapped: MainPlaylistController.goTo(index, true) }
        }
    }

    // A raised card for the compact skins
    component Card: Rectangle {
        color: theme.bg.secondary
        radius: VLCStyle.dp(16, VLCStyle.scale)
        border.color: theme.border
        border.width: VLCStyle.border
    }

    function hash(s) {
        let h = 0
        for (let i = 0; i < s.length; ++i)
            h = (h * 31 + s.charCodeAt(i)) >>> 0
        return h
    }

    // ---------- layouts ----------

    Item {
        id: area
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding

        // Album: cover and titles on the left, the album or queue on the right
        RowLayout {
            anchors.fill: parent
            visible: root.skin === "album"
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    anchors.centerIn: parent
                    width: parent.width - VLCStyle.margin_xlarge * 2
                    spacing: VLCStyle.margin_xlarge
                    Cover {
                        readonly property real side: Math.min(area.height * 0.6, parent.width * 0.38)
                        Layout.preferredWidth: side
                        Layout.preferredHeight: side
                    }
                    Titles { Layout.fillWidth: true; titleSize: VLCStyle.dp(36, VLCStyle.scale) }
                }
            }
            Rectangle {
                Layout.preferredWidth: Math.min(VLCStyle.dp(340, VLCStyle.scale), area.width * 0.35)
                Layout.fillHeight: true
                color: theme.bg.secondary
                visible: playlistModel.count > 1
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    Widgets.SubtitleLabel {
                        text: Player.album !== "" ? Player.album : qsTr("Up next")
                        Layout.margins: VLCStyle.margin_normal
                        color: theme.fg.primary
                    }
                    TrackList { Layout.fillWidth: true; Layout.fillHeight: true }
                }
            }
        }

        // Album compact: one card with cover, titles and a short track list
        Card {
            visible: root.skin === "albumcompact"
            anchors.centerIn: parent
            width: Math.min(area.width - VLCStyle.margin_large * 2, VLCStyle.dp(520, VLCStyle.scale))
            height: Math.min(area.height - VLCStyle.margin_large * 2, compactCol.implicitHeight + VLCStyle.margin_normal * 2)
            clip: true

            ColumnLayout {
                id: compactCol
                anchors.fill: parent
                anchors.margins: VLCStyle.margin_normal
                spacing: VLCStyle.margin_normal
                RowLayout {
                    spacing: VLCStyle.margin_normal
                    Cover {
                        Layout.preferredWidth: VLCStyle.dp(128, VLCStyle.scale)
                        Layout.preferredHeight: VLCStyle.dp(128, VLCStyle.scale)
                        radius: VLCStyle.dp(10, VLCStyle.scale)
                    }
                    Titles { Layout.fillWidth: true; titleSize: VLCStyle.fontSize_xlarge }
                }
                TrackList {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: Math.min(contentHeight, VLCStyle.dp(36 * 6, VLCStyle.scale))
                    visible: playlistModel.count > 1
                }
            }
        }

        // Turntable: the record on the left, titles on the right
        RowLayout {
            anchors.centerIn: parent
            width: area.width - VLCStyle.margin_xlarge * 2
            visible: root.skin === "turntable"
            spacing: VLCStyle.margin_xlarge
            Disc {
                readonly property real side: Math.min(area.height * 0.72, area.width * 0.42)
                Layout.preferredWidth: side
                Layout.preferredHeight: side
            }
            Titles { Layout.fillWidth: true; titleSize: VLCStyle.dp(34, VLCStyle.scale) }
        }

        // Turntable compact: a small record beside the titles, in a card
        Card {
            visible: root.skin === "turntablecompact"
            anchors.centerIn: parent
            width: Math.min(area.width - VLCStyle.margin_large * 2, VLCStyle.dp(520, VLCStyle.scale))
            height: VLCStyle.dp(200, VLCStyle.scale)
            RowLayout {
                anchors.fill: parent
                anchors.margins: VLCStyle.margin_normal
                spacing: VLCStyle.margin_normal
                Disc {
                    Layout.preferredWidth: VLCStyle.dp(164, VLCStyle.scale)
                    Layout.preferredHeight: VLCStyle.dp(164, VLCStyle.scale)
                }
                Titles { Layout.fillWidth: true; titleSize: VLCStyle.fontSize_xlarge }
            }
        }

        // Mini player: a slim strip
        Card {
            visible: root.skin === "mini"
            anchors.centerIn: parent
            width: Math.min(area.width - VLCStyle.margin_large * 2, VLCStyle.dp(480, VLCStyle.scale))
            height: VLCStyle.dp(96, VLCStyle.scale)
            RowLayout {
                anchors.fill: parent
                anchors.margins: VLCStyle.margin_small
                spacing: VLCStyle.margin_normal
                Cover {
                    Layout.preferredWidth: VLCStyle.dp(72, VLCStyle.scale)
                    Layout.preferredHeight: VLCStyle.dp(72, VLCStyle.scale)
                    radius: VLCStyle.dp(8, VLCStyle.scale)
                }
                Titles { Layout.fillWidth: true; titleSize: VLCStyle.fontSize_large }
            }
        }
    }
}
