/*****************************************************************************
 * VLXCommandPalette.qml: Ctrl+K command palette with every command and its keys
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

import VLC.MainInterface
import VLC.Style
import VLC.Dialogs
import VLC.Widgets as Widgets

// Lists VLC's hotkey actions (with the keys they are bound to now, remaps included) and the
// interface's own commands. Running a hotkey action does exactly what pressing its keys does.
FocusScope {
    id: root

    property bool open: false
    property var entries: []
    property var shown: []

    visible: opacity > 0
    opacity: open ? 1 : 0
    Behavior on opacity { NumberAnimation { duration: VLCStyle.duration_short; easing.type: open ? VLCStyle.easing_enter : VLCStyle.easing_exit } }

    readonly property ColorContext colorContext: ColorContext {
        id: theme
        colorSet: ColorContext.Window
    }
    // a raised surface on any theme: a light tint on dark backgrounds, a slight shade on light ones
    readonly property bool darkTheme: theme.bg.primary.hslLightness < 0.5
    readonly property color raised: Qt.tint(theme.bg.primary, darkTheme ? Qt.rgba(1, 1, 1, 0.07) : Qt.rgba(0, 0, 0, 0.03))
    readonly property color raisedMore: Qt.tint(theme.bg.primary, darkTheme ? Qt.rgba(1, 1, 1, 0.13) : Qt.rgba(0, 0, 0, 0.07))

    // the interface's own window shortcuts (menus/qml/GlobalShortcuts.qml), plus VLX pages
    readonly property var appCommands: [
        { id: "app-settings", label: qsTr("Open VLX settings"), keys: [], run: () => History.push(["vlxsettings"]) },
        { id: "app-open", label: qsTr("Open file"), keys: ["Ctrl+O"], run: () => DialogsProvider.simpleOpenDialog() },
        { id: "app-open-adv", label: qsTr("Open file (advanced)"), keys: ["Ctrl+Shift+O"], run: () => DialogsProvider.openFileDialog() },
        { id: "app-folder", label: qsTr("Open folder"), keys: ["Ctrl+F"], run: () => DialogsProvider.PLOpenDir() },
        { id: "app-disc", label: qsTr("Open disc"), keys: ["Ctrl+D"], run: () => DialogsProvider.openDiscDialog() },
        { id: "app-net", label: qsTr("Open network stream"), keys: ["Ctrl+N"], run: () => DialogsProvider.openNetDialog() },
        { id: "app-queue", label: qsTr("Show or hide the play queue"), keys: ["Ctrl+L"], run: () => { MainCtx.playqueuePanel.visible = !MainCtx.playqueuePanel.visible } },
        { id: "app-goto", label: qsTr("Go to time"), keys: ["Ctrl+T"], run: () => DialogsProvider.gotoTimeDialog() },
        { id: "app-info", label: qsTr("Media information"), keys: ["Ctrl+I"], run: () => DialogsProvider.mediaInfoDialog() },
        { id: "app-codec", label: qsTr("Codec information"), keys: ["Ctrl+J"], run: () => DialogsProvider.mediaCodecDialog() },
        { id: "app-effects", label: qsTr("Effects and filters"), keys: ["Ctrl+E"], run: () => DialogsProvider.extendedDialog() },
        { id: "app-messages", label: qsTr("Messages"), keys: ["Ctrl+M"], run: () => DialogsProvider.messagesDialog() },
        { id: "app-prefs", label: qsTr("Every VLC setting"), keys: ["Ctrl+P"], run: () => DialogsProvider.prefsDialog() },
        { id: "app-minimal", label: qsTr("Minimal view"), keys: ["Ctrl+H"], run: () => { MainCtx.minimalView = !MainCtx.minimalView } },
        { id: "app-ifs", label: qsTr("Interface fullscreen"), keys: ["F11"], run: () => MainCtx.toggleInterfaceFullScreen() },
        { id: "app-zoomin", label: qsTr("Bigger interface"), keys: ["Ctrl++"], run: () => MainCtx.incrementIntfUserScaleFactor(true) },
        { id: "app-zoomout", label: qsTr("Smaller interface"), keys: ["Ctrl+-"], run: () => MainCtx.incrementIntfUserScaleFactor(false) },
        { id: "app-help", label: qsTr("Help"), keys: ["F1"], run: () => DialogsProvider.helpDialog() },
        { id: "app-quit", label: qsTr("Quit"), keys: ["Ctrl+Q"], run: () => DialogsProvider.quit() }
    ]

    function toggle() {
        if (open) {
            close()
            return
        }
        entries = appCommands.concat(VLXSettings.commands())
        search.text = ""
        filter()
        open = true
        search.forceActiveFocus()
    }

    function close() {
        open = false
    }

    function filter() {
        const terms = search.text.toLowerCase().trim().split(/\s+/).filter(t => t !== "")
        shown = entries.filter(e => {
            const hay = (e.label + " " + e.keys.join(" ")).toLowerCase()
            return terms.every(t => hay.indexOf(t) >= 0)
        })
        list.currentIndex = 0
    }

    function run(entry) {
        close()
        Qt.callLater(() => {
            if (entry.run)
                entry.run()
            else
                VLXSettings.runCommand(entry.id)
        })
    }

    // --qt-vlx-start-view=palette opens it at start, for screenshots and tests
    Component.onCompleted: if (VLXSettings.startView === "palette") Qt.callLater(root.toggle)

    Rectangle { // scrim
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.55)
        TapHandler { onTapped: root.close() }
    }

    Rectangle { // the palette
        id: card
        width: Math.min(parent.width - VLCStyle.margin_xlarge * 2, VLCStyle.dp(640, VLCStyle.scale))
        height: Math.min(parent.height * 0.62, header.height + footer.height + list.contentHeight + VLCStyle.margin_small * 2)
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.14
        radius: VLCStyle.dp(14, VLCStyle.scale)
        color: root.raised
        border.width: VLCStyle.border
        border.color: theme.border
        clip: true

        scale: root.open ? 1 : 0.97
        Behavior on scale { NumberAnimation { duration: VLCStyle.duration_short; easing.type: VLCStyle.easing_enter } }

        Item { // search
            id: header
            width: parent.width
            height: VLCStyle.dp(52, VLCStyle.scale)

            TextInput {
                id: search
                anchors { fill: parent; leftMargin: VLCStyle.margin_normal; rightMargin: VLCStyle.margin_normal }
                verticalAlignment: TextInput.AlignVCenter
                color: theme.fg.primary
                selectionColor: theme.accent
                font.pixelSize: VLCStyle.fontSize_large
                onTextChanged: root.filter()
                Keys.onEscapePressed: root.close()
                Keys.onDownPressed: list.currentIndex = Math.min(list.currentIndex + 1, list.count - 1)
                Keys.onUpPressed: list.currentIndex = Math.max(list.currentIndex - 1, 0)
                Keys.onReturnPressed: if (root.shown.length) root.run(root.shown[list.currentIndex])
                Keys.onEnterPressed: if (root.shown.length) root.run(root.shown[list.currentIndex])
            }
            Widgets.ListLabel {
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: VLCStyle.margin_normal }
                visible: search.text === ""
                text: qsTr("Type a command or a key, e.g. fullscreen, subtitle, Ctrl")
                color: theme.fg.secondary
                font.pixelSize: VLCStyle.fontSize_large
            }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: VLCStyle.border; color: theme.border }
        }

        ListView {
            id: list
            anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: footer.top; margins: VLCStyle.margin_xxsmall }
            clip: true
            model: root.shown
            boundsBehavior: Flickable.StopAtBounds
            highlightMoveDuration: VLCStyle.duration_short
            keyNavigationEnabled: false

            delegate: Rectangle {
                id: row
                required property var modelData
                required property int index
                width: ListView.view.width
                height: VLCStyle.dp(38, VLCStyle.scale)
                radius: VLCStyle.button_radius
                color: (ListView.isCurrentItem || hover.hovered) ? root.raisedMore : "transparent"
                Behavior on color { ColorAnimation { duration: VLCStyle.duration_short } }

                Widgets.ListLabel {
                    anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: VLCStyle.margin_normal; right: caps.left; rightMargin: VLCStyle.margin_small }
                    text: row.modelData.label
                    elide: Text.ElideRight
                    color: theme.fg.primary
                }

                Row { // key caps, one group per binding
                    id: caps
                    anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: VLCStyle.margin_normal }
                    spacing: VLCStyle.margin_small
                    Repeater {
                        model: row.modelData.keys.slice(0, 2)
                        delegate: Row {
                            required property var modelData
                            spacing: VLCStyle.margin_xxxsmall
                            Repeater {
                                model: modelData === "+" ? ["+"] : modelData.split(/\+(?!$)/)
                                delegate: Rectangle {
                                    required property var modelData
                                    width: Math.max(height, capLabel.implicitWidth + VLCStyle.margin_small)
                                    height: VLCStyle.dp(22, VLCStyle.scale)
                                    radius: VLCStyle.dp(5, VLCStyle.scale)
                                    color: root.raised
                                    border.width: VLCStyle.border
                                    border.color: theme.border
                                    Widgets.CaptionLabel { id: capLabel; anchors.centerIn: parent; text: parent.modelData; color: theme.fg.primary }
                                }
                            }
                        }
                    }
                    Widgets.CaptionLabel {
                        visible: row.modelData.keys.length === 0
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("no keys")
                        color: theme.fg.secondary
                    }
                }

                HoverHandler { id: hover }
                TapHandler { onTapped: root.run(row.modelData) }
            }

            Widgets.ListLabel {
                anchors.centerIn: parent
                visible: list.count === 0
                text: qsTr("No command matches.")
                color: theme.fg.secondary
            }
        }
        Item { // key hints
            id: footer
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
            height: VLCStyle.dp(34, VLCStyle.scale)
            Rectangle { width: parent.width; height: VLCStyle.border; color: theme.border }
            Widgets.CaptionLabel {
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: VLCStyle.margin_normal }
                text: qsTr("↑ ↓ to move    Enter to run    Esc to close")
                color: theme.fg.secondary
            }
            Widgets.CaptionLabel {
                anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: VLCStyle.margin_normal }
                text: qsTr("%1 commands").arg(list.count)
                color: theme.fg.secondary
            }
        }
    }
}
