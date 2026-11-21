/*****************************************************************************
 * VLXSettingsPage.qml: VLX settings, with search and live previews (DESIGN.md › Settings)
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

import VLC.MainInterface
import VLC.Style
import VLC.Dialogs
import VLC.Widgets as Widgets

// Every change applies at once; the app itself is the preview for theme, accent and size,
// and the mini player at the top of Player shows the bar style.
FocusScope {
    id: root

    // behave like a Page
    property var pagePrefix: []

    readonly property ColorContext colorContext: ColorContext {
        id: theme
        colorSet: ColorContext.View
    }

    // ---------- the settings, as data (also what search looks through) ----------

    readonly property var sections: [
        { id: "look", name: qsTr("Look") },
        { id: "player", name: qsTr("Player") },
        { id: "audio", name: qsTr("Audio player") },
        { id: "files", name: qsTr("File icons") },
        { id: "advanced", name: qsTr("Advanced") }
    ]

    readonly property var settings: [
        { id: "theme", section: "look", name: qsTr("Theme"), help: qsTr("Colours for the whole app."), keys: "dark light mode colour color scheme appearance",
          options: [["default", qsTr("VLC default")], ["dark", qsTr("Dark")], ["light", qsTr("Light")], ["paper", qsTr("Paper")], ["ink", qsTr("Ink")], ["nord", qsTr("Nord")], ["mocha", qsTr("Mocha")], ["tokyo", qsTr("Tokyo")]],
          get: () => VLXSettings.theme, set: (v) => { VLXSettings.theme = v } },
        { id: "accent", section: "look", name: qsTr("Accent colour"), help: qsTr("Progress, selection and buttons. Adjusted so it stays readable on the theme."), keys: "color highlight tint",
          options: [["default", qsTr("Theme")], ["#FF8800", qsTr("VLC orange")], ["#E6B450", qsTr("Amber")], ["#E0466A", qsTr("Rose")], ["#5B8CFF", qsTr("Blue")], ["#10B981", qsTr("Emerald")], ["#8B5CF6", qsTr("Grape")], ["#9F234A", qsTr("Wine")], ["system", qsTr("Windows")]],
          get: () => VLXSettings.accent, set: (v) => { VLXSettings.accent = v } },
        { id: "scale", section: "look", name: qsTr("Interface size"), help: qsTr("Text, controls and spacing together. Ctrl + and Ctrl − also work."), keys: "zoom scale dpi bigger smaller",
          options: [["0.8", "80 %"], ["0.9", "90 %"], ["1", "100 %"], ["1.1", "110 %"], ["1.25", "125 %"], ["1.5", "150 %"]],
          get: () => String(+MainCtx.getIntfUserScaleFactor().toFixed(2)), set: (v) => MainCtx.setIntfUserScaleFactor(+v) },
        { id: "barStyle", section: "player", name: qsTr("Bar style"), help: qsTr("How the player controls look."), keys: "controls toolbar island docked minimal pill classic bottom bar",
          options: [["island", qsTr("Island")], ["docked", qsTr("Docked bar")], ["minimal", qsTr("Minimal")], ["pill", qsTr("Compact pill")], ["classic", qsTr("Classic")]],
          get: () => VLXSettings.barStyle, set: (v) => { VLXSettings.barStyle = v } },
        { id: "barPosition", section: "player", name: qsTr("Controls position"), help: qsTr("Dock the player controls at the bottom or at the top, under the title."), keys: "top bottom dock controls position",
          options: [["bottom", qsTr("Bottom")], ["top", qsTr("Top")]],
          get: () => VLXSettings.barPosition, set: (v) => { VLXSettings.barPosition = v } },
        { id: "openMode", section: "player", name: qsTr("Opening files"), help: qsTr("What happens when you open a video from Explorer while VLX is already running. A new window lets you watch several at once."),
          keys: "instance instances multiple windows several one instance new window explorer double click queue enqueue",
          options: [["new", qsTr("A new window for each file")], ["running", qsTr("Play in the open window")], ["queue", qsTr("Queue in the open window")]],
          get: () => VLXSettings.openMode(), set: (v) => VLXSettings.setOpenMode(v) },
        { id: "buttons", section: "player", name: qsTr("Player buttons"), help: qsTr("Choose and arrange the buttons on the video, audio and mini player bars, with profiles."),
          keys: "toolbar buttons customise customize editor controls layout bar profile drag",
          action: qsTr("Customise"), run: () => DialogsProvider.showToolbarEditorDialog() },
        { id: "hideDelay", section: "player", name: qsTr("Hide controls after"), help: qsTr("How long the controls and the mouse pointer stay after the mouse stops moving."), keys: "auto hide timeout mouse pointer cursor",
          options: [["1000", "1 s"], ["2000", "2 s"], ["3000", "3 s"], ["5000", "5 s"], ["86400000", qsTr("Never")]],
          get: () => String(VLXSettings.intOption("mouse-hide-timeout")), set: (v) => VLXSettings.setIntOption("mouse-hide-timeout", +v) },
        { id: "audioSkin", section: "audio", name: qsTr("Audio player skin"), help: qsTr("What the player shows for files without video."), keys: "music skin album turntable vinyl record mini",
          options: [["albumcompact", qsTr("Album compact")], ["album", qsTr("Album")], ["turntable", qsTr("Turntable")], ["turntablecompact", qsTr("Turntable compact")], ["mini", qsTr("Mini player")], ["classic", qsTr("VLC classic")]],
          get: () => VLXSettings.audioSkin, set: (v) => { VLXSettings.audioSkin = v } },
        { id: "audioBackground", section: "audio", name: qsTr("Background"), help: qsTr("The theme's plain background, or a soft wash of the cover art's colours."), keys: "tint tinted blur cover art colour color background",
          options: [["plain", qsTr("Plain theme")], ["tinted", qsTr("Tinted from the cover art")]],
          get: () => VLXSettings.audioBackground, set: (v) => { VLXSettings.audioBackground = v } },
        { id: "audioTheme", section: "audio", name: qsTr("Colours"), help: qsTr("Follow the app theme, or give the audio player a theme of its own."), keys: "theme colour color dark light own audio music",
          options: [["app", qsTr("Follow the app theme")], ["dark", qsTr("Dark")], ["light", qsTr("Light")], ["paper", qsTr("Paper")], ["ink", qsTr("Ink")], ["nord", qsTr("Nord")], ["mocha", qsTr("Mocha")], ["tokyo", qsTr("Tokyo")]],
          get: () => VLXSettings.audioTheme, set: (v) => { VLXSettings.audioTheme = v } },
        { id: "fileIcons", section: "files", name: qsTr("File icon style"), help: qsTr("Icons for media files in Explorer."), keys: "explorer icons file types association",
          options: [["band", qsTr("Page and band")], ["echo", qsTr("Echo")]],
          get: () => VLXSettings.fileIcons, set: (v) => { VLXSettings.fileIcons = v } },
        { id: "fileIconLook", section: "files", name: qsTr("File icon look"), help: qsTr("Light or dark icons, or follow the Windows app theme."), keys: "explorer icons dark light",
          options: [["auto", qsTr("Follow Windows")], ["light", qsTr("Light")], ["dark", qsTr("Dark")]],
          get: () => VLXSettings.fileIconLook, set: (v) => { VLXSettings.fileIconLook = v } },
        { id: "allSettings", section: "advanced", name: qsTr("Every VLC setting"), help: qsTr("VLC's full preferences, for the rare deep change: codecs, hardware decoding, network caching, hotkeys and more."),
          keys: "advanced preferences expert gpu hardware decoding codec network caching buffer hotkeys shortcuts subtitles audio output video output",
          action: qsTr("Open"), run: () => DialogsProvider.prefsDialog() }
    ]

    property int revision: 0 // bumped after a change so option chips re-read their state
    property string flashId: ""

    function sectionName(id) {
        return sections.find(s => s.id === id)?.name ?? ""
    }

    function matches(query) {
        const terms = query.toLowerCase().trim().split(/\s+/).filter(t => t !== "")
        if (terms.length === 0)
            return []
        return settings.filter(s => {
            const hay = (s.name + " " + s.help + " " + (s.keys ?? "") + " " + sectionName(s.section)).toLowerCase()
            return terms.every(t => hay.indexOf(t) >= 0)
        }).slice(0, 6)
    }

    function jumpTo(id) {
        const item = rowRepeater.itemAt(settings.findIndex(s => s.id === id))
        if (!item)
            return
        flick.contentY = Math.max(0, Math.min(item.y - VLCStyle.margin_normal, flick.contentHeight - flick.height))
        flashId = ""
        flashId = id
        flashTimer.restart()
    }

    Timer { id: flashTimer; interval: 1400; onTriggered: root.flashId = "" }

    Rectangle { anchors.fill: parent; color: theme.bg.primary }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ---------- sections ----------
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: VLCStyle.dp(200, VLCStyle.scale)
            color: theme.bg.secondary

            Column {
                anchors.fill: parent
                anchors.margins: VLCStyle.margin_small
                spacing: VLCStyle.margin_xxxsmall
                Widgets.SubtitleLabel { text: qsTr("Settings"); color: theme.fg.primary; padding: VLCStyle.margin_small }
                Repeater {
                    model: root.sections
                    delegate: Rectangle {
                        required property var modelData
                        width: parent.width
                        height: VLCStyle.dp(34, VLCStyle.scale)
                        radius: VLCStyle.button_radius
                        color: secHover.hovered ? theme.bg.primary : "transparent"
                        Behavior on color { ColorAnimation { duration: VLCStyle.duration_short } }
                        Widgets.ListLabel { anchors.verticalCenter: parent.verticalCenter; x: VLCStyle.margin_small; text: modelData.name; color: theme.fg.primary }
                        HoverHandler { id: secHover }
                        TapHandler {
                            onTapped: {
                                const first = root.settings.find(s => s.section === modelData.id)
                                if (first) root.jumpTo(first.id)
                            }
                        }
                    }
                }
            }
        }

        // ---------- search and settings ----------
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle { // search box
                id: searchBox
                anchors { left: parent.left; right: parent.right; top: parent.top; margins: VLCStyle.margin_normal }
                height: VLCStyle.dp(40, VLCStyle.scale)
                radius: VLCStyle.button_radius
                color: theme.bg.secondary
                border.width: VLCStyle.border
                border.color: search.activeFocus ? theme.accent : theme.border
                Behavior on border.color { ColorAnimation { duration: VLCStyle.duration_short } }

                TextInput {
                    id: search
                    anchors { fill: parent; leftMargin: VLCStyle.margin_normal; rightMargin: VLCStyle.margin_normal }
                    verticalAlignment: TextInput.AlignVCenter
                    color: theme.fg.primary
                    selectionColor: theme.accent
                    font.pixelSize: VLCStyle.fontSize_normal
                    focus: true
                    property int active: 0
                    readonly property var hits: root.matches(text)
                    onTextChanged: active = 0
                    Keys.onDownPressed: active = Math.min(active + 1, hits.length - 1)
                    Keys.onUpPressed: active = Math.max(active - 1, 0)
                    Keys.onReturnPressed: if (hits.length) { root.jumpTo(hits[active].id); text = "" }
                    Keys.onEscapePressed: text = ""
                }
                Widgets.ListLabel {
                    anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: VLCStyle.margin_normal }
                    visible: search.text === ""
                    text: qsTr("Search settings: try gpu, subtitles, dark, buffer")
                    color: theme.fg.secondary
                }
            }

            Rectangle { // results
                z: 2
                visible: search.text !== ""
                anchors { left: searchBox.left; right: searchBox.right; top: searchBox.bottom; topMargin: VLCStyle.margin_xxsmall }
                height: resultCol.implicitHeight + VLCStyle.margin_xsmall * 2
                radius: VLCStyle.dp(10, VLCStyle.scale)
                color: theme.bg.secondary
                border.width: VLCStyle.border
                border.color: theme.border

                Column {
                    id: resultCol
                    anchors { left: parent.left; right: parent.right; top: parent.top; margins: VLCStyle.margin_xsmall }
                    Repeater {
                        model: search.hits
                        delegate: Rectangle {
                            required property var modelData
                            required property int index
                            width: parent.width
                            height: VLCStyle.dp(34, VLCStyle.scale)
                            radius: VLCStyle.button_radius
                            color: (index === search.active || hitHover.hovered) ? theme.bg.primary : "transparent"
                            Widgets.ListLabel { anchors.verticalCenter: parent.verticalCenter; x: VLCStyle.margin_small; text: modelData.name; color: theme.fg.primary }
                            Widgets.CaptionLabel { anchors.verticalCenter: parent.verticalCenter; anchors.right: parent.right; anchors.rightMargin: VLCStyle.margin_small; text: root.sectionName(modelData.section); color: theme.fg.secondary }
                            HoverHandler { id: hitHover }
                            TapHandler { onTapped: { root.jumpTo(modelData.id); search.text = "" } }
                        }
                    }
                    Widgets.ListLabel {
                        visible: search.hits.length === 0
                        padding: VLCStyle.margin_small
                        text: qsTr("No VLX setting matches. Advanced › Every VLC setting has the rest.")
                        color: theme.fg.secondary
                    }
                }
            }

            Flickable {
                id: flick
                anchors { left: parent.left; right: parent.right; top: searchBox.bottom; bottom: parent.bottom; topMargin: VLCStyle.margin_normal }
                contentHeight: listCol.implicitHeight + VLCStyle.margin_xlarge
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                Behavior on contentY { NumberAnimation { duration: VLCStyle.duration_view; easing.type: VLCStyle.easing_enter } }

                Column {
                    id: listCol
                    x: VLCStyle.margin_normal
                    width: flick.width - VLCStyle.margin_normal * 2
                    spacing: VLCStyle.margin_small

                    // live preview of the bar style, above the Player settings
                    Repeater {
                        id: rowRepeater
                        model: root.settings
                        delegate: Column {
                            id: settingRow
                            required property var modelData
                            required property int index
                            width: listCol.width
                            spacing: VLCStyle.margin_xsmall

                            Widgets.SubtitleLabel { // section heading before the first setting of each section
                                visible: index === 0 || root.settings[index - 1].section !== modelData.section
                                topPadding: index === 0 ? 0 : VLCStyle.margin_normal
                                text: root.sectionName(modelData.section)
                                color: theme.fg.primary
                            }

                            Item { // audio player preview: the chosen skin in the chosen colours and background
                                id: audioPreview
                                visible: modelData.id === "audioSkin"
                                width: Math.min(parent.width, VLCStyle.dp(420, VLCStyle.scale))
                                height: visible ? width * 0.42 : 0
                                readonly property var own: VLXSettings.audioTheme !== "app" ? VLXSettings.themePalette(VLXSettings.audioTheme) : null
                                readonly property color cBg: own ? own.bg : theme.bg.primary
                                readonly property color cPanel: own ? own.panel : theme.bg.secondary
                                readonly property color cLine: own ? own.line : theme.border
                                readonly property color cText: own ? own.text : theme.fg.primary
                                readonly property color cMuted: own ? own.muted : theme.fg.secondary
                                readonly property color cAccent: own ? own.accent : theme.accent
                                readonly property string sk: VLXSettings.audioSkin
                                readonly property bool disc: sk === "turntable" || sk === "turntablecompact"
                                readonly property bool compact: sk === "albumcompact" || sk === "turntablecompact"
                                readonly property real u: width / 42 // layout unit

                                Rectangle {
                                    anchors.fill: parent
                                    radius: VLCStyle.dp(12, VLCStyle.scale)
                                    color: audioPreview.cBg
                                    border.color: audioPreview.cLine
                                    border.width: VLCStyle.border
                                    clip: true
                                    Behavior on color { ColorAnimation { duration: VLCStyle.duration_long } }

                                    Rectangle { // "tinted from the cover art"
                                        anchors.fill: parent
                                        radius: parent.radius
                                        visible: VLXSettings.audioBackground === "tinted"
                                        gradient: Gradient {
                                            GradientStop { position: 0; color: Qt.hsla(0.06, 0.6, 0.5, 0.45) }
                                            GradientStop { position: 1; color: Qt.alpha(audioPreview.cBg, 0.6) }
                                        }
                                    }

                                    Rectangle { // compact skins sit in a card
                                        visible: audioPreview.compact
                                        anchors.centerIn: parent
                                        width: parent.width * 0.7; height: parent.height * 0.52
                                        radius: audioPreview.u
                                        color: audioPreview.cPanel
                                        border.color: audioPreview.cLine; border.width: VLCStyle.border
                                    }

                                    Item { // cover or record
                                        id: art
                                        readonly property real side: audioPreview.sk === "mini" ? parent.height * 0.18
                                                                   : audioPreview.compact ? parent.height * 0.38 : parent.height * 0.66
                                        visible: audioPreview.sk !== "classic"
                                        width: side; height: side
                                        x: audioPreview.sk === "mini" ? parent.width * 0.12 + audioPreview.u * 0.6
                                           : audioPreview.compact ? parent.width * 0.15 + audioPreview.u * 1.2 : audioPreview.u * 3
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.verticalCenterOffset: audioPreview.compact || audioPreview.sk === "mini" ? 0 : -audioPreview.u
                                        Rectangle { // record
                                            visible: audioPreview.disc
                                            anchors.fill: parent; radius: width / 2
                                            color: Qt.rgba(0.07, 0.07, 0.08, 1)
                                            Rectangle { anchors.centerIn: parent; width: parent.width * 0.4; height: width; radius: width / 2; color: audioPreview.cAccent }
                                            RotationAnimator on rotation { from: 0; to: 360; duration: 3200; loops: Animation.Infinite; running: audioPreview.visible && audioPreview.disc }
                                        }
                                        Rectangle { // cover
                                            visible: !audioPreview.disc
                                            anchors.fill: parent; radius: audioPreview.u * 0.6
                                            gradient: Gradient {
                                                GradientStop { position: 0; color: Qt.hsla(0.06, 0.62, 0.52, 1) }
                                                GradientStop { position: 1; color: Qt.hsla(0.1, 0.5, 0.25, 1) }
                                            }
                                        }
                                    }

                                    Column { // title, artist
                                        visible: audioPreview.sk !== "classic"
                                        anchors.verticalCenter: art.verticalCenter
                                        x: art.x + art.width + audioPreview.u * (audioPreview.sk === "mini" ? 0.8 : 1.5)
                                        spacing: audioPreview.u * 0.5
                                        Rectangle { width: audioPreview.u * (audioPreview.sk === "mini" ? 7 : 10); height: audioPreview.u * 0.9; radius: height / 2; color: audioPreview.cText }
                                        Rectangle { width: audioPreview.u * 6; height: audioPreview.u * 0.7; radius: height / 2; color: audioPreview.cMuted; visible: audioPreview.sk !== "mini" }
                                        Repeater { // up next / the album
                                            model: audioPreview.sk === "album" || audioPreview.sk === "turntable" ? 3 : 0
                                            Rectangle { width: audioPreview.u * 12; height: audioPreview.u * 0.6; radius: height / 2; color: Qt.alpha(audioPreview.cMuted, 0.5) }
                                        }
                                    }

                                    Rectangle { // the mini player is one slim strip
                                        visible: audioPreview.sk === "mini"
                                        z: -1
                                        anchors.centerIn: parent
                                        width: parent.width * 0.76; height: parent.height * 0.3
                                        radius: height / 2
                                        color: audioPreview.cPanel
                                        border.color: audioPreview.cLine; border.width: VLCStyle.border
                                    }

                                    Widgets.IconLabel { // VLC classic: the plain music view
                                        visible: audioPreview.sk === "classic"
                                        anchors.centerIn: parent
                                        text: VLCIcons.topbar_music
                                        font.pixelSize: parent.height * 0.3
                                        color: audioPreview.cMuted
                                    }

                                    Rectangle { // seek line
                                        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; margins: audioPreview.u * 2 }
                                        height: Math.max(2, audioPreview.u * 0.35); radius: height / 2
                                        color: Qt.alpha(audioPreview.cText, 0.2)
                                        Rectangle { width: parent.width * 0.38; height: parent.height; radius: height / 2; color: audioPreview.cAccent }
                                    }
                                }
                            }

                            Item { // bar style preview
                                visible: modelData.id === "barStyle"
                                width: Math.min(parent.width, VLCStyle.dp(420, VLCStyle.scale))
                                height: visible ? width * 0.42 : 0
                                Rectangle {
                                    anchors.fill: parent
                                    radius: VLCStyle.dp(12, VLCStyle.scale)
                                    gradient: Gradient {
                                        GradientStop { position: 0; color: Qt.rgba(0.77, 0.42, 0.18, 1) }
                                        GradientStop { position: 1; color: Qt.rgba(0.06, 0.1, 0.13, 1) }
                                    }
                                }
                                Rectangle {
                                    readonly property string s: VLXSettings.barStyle
                                    readonly property bool floating: s === "island" || s === "pill"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    readonly property bool atTop: VLXSettings.barPosition === "top"
                                    anchors.bottom: atTop ? undefined : parent.bottom
                                    anchors.top: atTop ? parent.top : undefined
                                    anchors.bottomMargin: floating ? parent.height * 0.08 : 0
                                    anchors.topMargin: floating ? parent.height * 0.08 : 0
                                    width: s === "pill" ? parent.width * 0.5 : s === "island" ? parent.width * 0.8 : parent.width
                                    height: s === "pill" ? parent.height * 0.16 : parent.height * 0.26
                                    radius: s === "pill" ? height / 2 : s === "island" ? VLCStyle.dp(10, VLCStyle.scale) : 0
                                    color: theme.bg.primary
                                    opacity: s === "minimal" ? 0 : s === "docked" ? 1 : 0.94
                                    Behavior on width { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                    Behavior on height { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                    Behavior on opacity { NumberAnimation { duration: VLCStyle.duration_long } }
                                    Rectangle { // seek line
                                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter; margins: parent.height * 0.5 }
                                        height: Math.max(2, parent.height * 0.08); radius: height / 2
                                        color: Qt.alpha(theme.fg.primary, 0.2)
                                        Rectangle { width: parent.width * 0.38; height: parent.height; radius: height / 2; color: theme.accent }
                                    }
                                }
                            }

                            Rectangle { // the setting's card
                                width: parent.width
                                height: cardCol.implicitHeight + VLCStyle.margin_normal * 2
                                radius: VLCStyle.dp(12, VLCStyle.scale)
                                color: theme.bg.secondary
                                border.width: root.flashId === modelData.id ? VLCStyle.dp(2, VLCStyle.scale) : VLCStyle.border
                                border.color: root.flashId === modelData.id ? theme.accent : theme.border
                                Behavior on border.color { ColorAnimation { duration: VLCStyle.duration_long } }

                                Column {
                                    id: cardCol
                                    anchors { left: parent.left; right: parent.right; top: parent.top; margins: VLCStyle.margin_normal }
                                    spacing: VLCStyle.margin_xsmall
                                    Widgets.ListLabel { text: modelData.name; color: theme.fg.primary; font.weight: Font.DemiBold }
                                    Widgets.CaptionLabel { width: parent.width; text: modelData.help; color: theme.fg.secondary; wrapMode: Text.Wrap }
                                    Flow {
                                        width: parent.width
                                        spacing: VLCStyle.margin_xsmall
                                        visible: modelData.options !== undefined
                                        Repeater {
                                            model: modelData.options ?? []
                                            delegate: Rectangle {
                                                required property var modelData
                                                readonly property var setting: settingRow.modelData
                                                readonly property bool selected: (root.revision, setting.get() === modelData[0])
                                                width: chipLabel.implicitWidth + VLCStyle.margin_normal * 2
                                                height: VLCStyle.dp(30, VLCStyle.scale)
                                                radius: height / 2
                                                color: selected ? theme.accent : (chipHover.hovered ? theme.bg.primary : "transparent")
                                                border.width: VLCStyle.border
                                                border.color: selected ? theme.accent : theme.border
                                                Behavior on color { ColorAnimation { duration: VLCStyle.duration_short } }
                                                Widgets.ListLabel {
                                                    id: chipLabel
                                                    anchors.centerIn: parent
                                                    text: modelData[1]
                                                    color: parent.selected ? theme.bg.primary : theme.fg.primary
                                                }
                                                HoverHandler { id: chipHover }
                                                TapHandler { onTapped: { setting.set(modelData[0]); root.revision++ } }
                                            }
                                        }
                                    }
                                    Rectangle { // action button (Advanced)
                                        visible: modelData.action !== undefined
                                        width: actLabel.implicitWidth + VLCStyle.margin_large * 2
                                        height: VLCStyle.dp(32, VLCStyle.scale)
                                        radius: VLCStyle.button_radius
                                        color: actHover.hovered ? Qt.lighter(theme.accent, 1.1) : theme.accent
                                        Widgets.ListLabel { id: actLabel; anchors.centerIn: parent; text: modelData.action ?? ""; color: theme.bg.primary; font.weight: Font.DemiBold }
                                        HoverHandler { id: actHover }
                                        TapHandler { onTapped: modelData.run() }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // a change made elsewhere (keyboard zoom, another window) refreshes the chips
    Connections {
        target: VLXSettings
        function onThemeChanged() { root.revision++ }
        function onBarStyleChanged() { root.revision++ }
        function onBarPositionChanged() { root.revision++ }
        function onAudioSkinChanged() { root.revision++ }
        function onFileIconsChanged() { root.revision++ }
    }
    Connections {
        target: MainCtx
        function onIntfScaleFactorChanged() { root.revision++ }
    }
}
