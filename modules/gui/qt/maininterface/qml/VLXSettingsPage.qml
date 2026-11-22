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
        { id: "performance", name: qsTr("Performance") },
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
        { id: "barAlign", section: "player", name: qsTr("Controls alignment"), help: qsTr("Where the play, previous and next buttons sit on the bar."), keys: "align alignment left center centre right controls buttons layout",
          options: [["center", qsTr("Centered")], ["left", qsTr("Left")], ["right", qsTr("Right")]],
          get: () => VLXSettings.barAlign, set: (v) => { VLXSettings.barAlign = v } },
        { id: "barSize", section: "player", name: qsTr("Bar size"), help: qsTr("How tall the player controls are. Compact keeps them slim."), keys: "compact small large big height slim bar size controls",
          options: [["compact", qsTr("Compact")], ["normal", qsTr("Normal")], ["large", qsTr("Large")]],
          get: () => VLXSettings.barSize, set: (v) => { VLXSettings.barSize = v } },
        { id: "pinControls", section: "player", name: qsTr("Always show the controls"), help: qsTr("Keep the controls under the video, always visible, as in VLC 3. Off: they float over the video and hide."), keys: "always show pin pinned dock controls bar visible hide vlc 3 under below",
          options: [["false", qsTr("Over the video, auto-hide")], ["true", qsTr("Always, under the video")]],
          get: () => String(VLXSettings.pinControls), set: (v) => { VLXSettings.pinControls = (v === "true") } },
        { id: "pinTopBar", section: "player", name: qsTr("Always show the top bar"), help: qsTr("Keep the title bar above the video, always visible. Off: it floats over the video and hides."), keys: "always show pin pinned top bar title visible hide",
          options: [["false", qsTr("Over the video, auto-hide")], ["true", qsTr("Always, above the video")]],
          get: () => String(VLXSettings.pinTopBar), set: (v) => { VLXSettings.pinTopBar = (v === "true") } },
        { id: "hideDelay", section: "player", name: qsTr("Hide controls after"), help: qsTr("How long the controls and the mouse pointer stay after the mouse stops moving."), keys: "auto hide timeout mouse pointer cursor",
          options: [["1000", "1 s"], ["2000", "2 s"], ["3000", "3 s"], ["5000", "5 s"], ["86400000", qsTr("Never")]],
          get: () => String(VLXSettings.intOption("mouse-hide-timeout")), set: (v) => VLXSettings.setIntOption("mouse-hide-timeout", +v) },
        { id: "openMode", section: "player", name: qsTr("Opening files"), help: qsTr("What happens when you open a video from Explorer while VLX is already running. A new window lets you watch several at once."),
          keys: "instance instances multiple windows several one instance new window explorer double click queue enqueue",
          options: [["new", qsTr("A new window for each file")], ["running", qsTr("Play in the open window")], ["queue", qsTr("Queue in the open window")]],
          get: () => VLXSettings.openMode(), set: (v) => VLXSettings.setOpenMode(v) },
        { id: "buttons", section: "player", name: qsTr("Player buttons"), help: qsTr("Choose and arrange the buttons on the video, audio and mini player bars, with profiles."),
          keys: "toolbar buttons customise customize editor controls layout bar profile drag",
          action: qsTr("Customise"), run: () => DialogsProvider.showToolbarEditorDialog() },
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
        { id: "memorySaver", section: "performance", name: qsTr("Memory saver"),
          help: qsTr("Decode with 2 threads and read 0.3 s ahead instead of 1 s: about 50 MB less when playing 1080p. Applies to the next file you open. CPU-only decoding of heavy 4K files may stutter."),
          keys: "memory ram usage lighter low saver threads cache caching buffer performance",
          options: [["false", qsTr("Off")], ["true", qsTr("On")]],
          get: () => String(VLXSettings.memorySaver()), set: (v) => VLXSettings.setMemorySaver(v === "true") },
        { id: "mediaLibrary", section: "performance", name: qsTr("Media library"),
          help: qsTr("Indexes your videos and music for the Video and Music pages. Off saves about 10 MB; Home then shows a simple open-file page. Takes effect the next time VLX starts."),
          keys: "media library index videos music collection memory ram performance scan",
          options: [["true", qsTr("On")], ["false", qsTr("Off")]],
          get: () => String(VLXSettings.mediaLibrary()), set: (v) => VLXSettings.setMediaLibrary(v === "true") },
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

    // --qt-vlx-start-view=vlxsettings:<setting id> opens here scrolled to that setting, for screenshots and tests
    Timer {
        interval: 600
        running: VLXSettings.startView.startsWith("vlxsettings:")
        onTriggered: root.jumpTo(VLXSettings.startView.split(":")[1])
    }

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
                                            border.color: Qt.rgba(1, 1, 1, 0.24); border.width: 1 // a rim, so it reads on dark cards
                                            Repeater { // grooves
                                                model: 3
                                                Rectangle {
                                                    required property int index
                                                    anchors.centerIn: parent
                                                    width: parent.width * (0.82 - index * 0.12); height: width; radius: width / 2
                                                    color: "transparent"; border.color: Qt.rgba(1, 1, 1, 0.09); border.width: 1
                                                }
                                            }
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

                            Item { // player preview: bar style, position, alignment, size and the always-show switches
                                id: barPreview
                                visible: modelData.id === "barStyle"
                                width: Math.min(parent.width, VLCStyle.dp(420, VLCStyle.scale))
                                height: visible ? width * 0.5 : 0
                                readonly property string s: VLXSettings.barStyle
                                readonly property string align: VLXSettings.barAlign
                                readonly property bool pinBar: VLXSettings.pinControls
                                readonly property bool pinTop: VLXSettings.pinTopBar
                                readonly property bool atTop: VLXSettings.barPosition === "top"
                                readonly property bool floating: (s === "island" || s === "pill") && !pinBar
                                readonly property real sizeK: VLXSettings.barSize === "compact" ? 0.62 : VLXSettings.barSize === "large" ? 1.25 : 1
                                readonly property real barH: height * ((s === "pill" && !pinBar) ? 0.17 : 0.24) * sizeK
                                readonly property real topH: height * 0.1
                                readonly property real r: VLCStyle.dp(12, VLCStyle.scale)

                                Rectangle { // window
                                    anchors.fill: parent
                                    radius: barPreview.r
                                    color: theme.bg.primary
                                }
                                Rectangle { // the video, between whichever bars are always shown
                                    id: pVideo
                                    readonly property real t: (barPreview.pinTop ? barPreview.topH : 0) + ((barPreview.pinBar && barPreview.atTop) ? barPreview.barH : 0)
                                    readonly property real b: (barPreview.pinBar && !barPreview.atTop) ? barPreview.barH : 0
                                    x: 0; y: t
                                    width: parent.width
                                    height: parent.height - t - b
                                    radius: (t === 0 && b === 0) ? barPreview.r : 0
                                    gradient: Gradient {
                                        GradientStop { position: 0; color: Qt.rgba(0.77, 0.42, 0.18, 1) }
                                        GradientStop { position: 1; color: Qt.rgba(0.06, 0.1, 0.13, 1) }
                                    }
                                    Behavior on y { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                    Behavior on height { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                }
                                Rectangle { // top bar, when always shown
                                    visible: barPreview.pinTop
                                    width: parent.width; height: barPreview.topH
                                    color: theme.bg.secondary
                                    Rectangle { x: parent.height * 0.6; anchors.verticalCenter: parent.verticalCenter; width: parent.width * 0.3; height: parent.height * 0.22; radius: height / 2; color: Qt.alpha(theme.fg.primary, 0.5) }
                                }
                                Rectangle { // the bar
                                    id: pBar
                                    readonly property real inset: barPreview.floating ? barPreview.height * 0.06 : 0
                                    width: barPreview.floating ? (barPreview.s === "pill" ? parent.width * 0.5 : parent.width * 0.8) : parent.width
                                    height: barPreview.barH
                                    x: (parent.width - width) / 2
                                    y: barPreview.pinBar ? (barPreview.atTop ? (barPreview.pinTop ? barPreview.topH : 0) : parent.height - height)
                                       : barPreview.atTop ? pVideo.y + inset
                                       : pVideo.y + pVideo.height - height - inset
                                    radius: !barPreview.floating ? 0 : barPreview.s === "pill" ? height / 2 : VLCStyle.dp(10, VLCStyle.scale)
                                    color: Qt.alpha((barPreview.pinBar || barPreview.s === "docked") ? theme.bg.secondary : theme.bg.primary,
                                                    (barPreview.s === "minimal" && !barPreview.pinBar) ? 0 : (barPreview.pinBar || barPreview.s === "docked") ? 1 : 0.94)
                                    Behavior on width { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                    Behavior on height { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                    Behavior on y { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }

                                    Rectangle { // seek line
                                        anchors { left: parent.left; right: parent.right; leftMargin: parent.height * 0.4; rightMargin: parent.height * 0.4 }
                                        y: barPreview.s === "pill" && !barPreview.pinBar ? parent.height * 0.5 - height / 2 : parent.height * 0.22
                                        height: Math.max(2, parent.height * 0.07); radius: height / 2
                                        color: Qt.alpha(theme.fg.primary, 0.2)
                                        Rectangle { width: parent.width * 0.38; height: parent.height; radius: height / 2; color: theme.accent }
                                    }
                                    Row { // playback group: previous, play, next
                                        id: pPlay
                                        visible: !(barPreview.s === "pill" && !barPreview.pinBar)
                                        spacing: parent.height * 0.14
                                        y: parent.height * 0.62 - height / 2
                                        x: barPreview.align === "left" ? parent.height * 0.4
                                           : barPreview.align === "right" ? parent.width - width - parent.height * 0.4
                                           : (parent.width - width) / 2
                                        Behavior on x { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                        Repeater {
                                            model: 3
                                            Rectangle {
                                                required property int index
                                                anchors.verticalCenter: parent.verticalCenter
                                                width: index === 1 ? pBar.height * 0.42 : pBar.height * 0.18
                                                height: width; radius: width / 2
                                                color: index === 1 ? "transparent" : theme.fg.primary
                                                border.color: theme.accent
                                                border.width: index === 1 ? Math.max(2, width * 0.12) : 0
                                            }
                                        }
                                    }
                                    Row { // the other buttons
                                        visible: pPlay.visible
                                        spacing: parent.height * 0.12
                                        y: parent.height * 0.62 - height / 2
                                        x: barPreview.align === "right" ? parent.height * 0.4 : parent.width - width - parent.height * 0.4
                                        Behavior on x { NumberAnimation { duration: VLCStyle.duration_long; easing.type: VLCStyle.easing_enter } }
                                        Repeater {
                                            model: 3
                                            Rectangle { width: pBar.height * 0.16; height: width; radius: width * 0.3; color: Qt.alpha(theme.fg.primary, 0.75) }
                                        }
                                    }
                                }
                                Rectangle { // frame
                                    anchors.fill: parent
                                    radius: barPreview.r
                                    color: "transparent"
                                    border.color: theme.border
                                    border.width: VLCStyle.border
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
