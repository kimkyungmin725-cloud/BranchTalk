import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window

    objectName: "mainWindow"
    readonly property bool darkMode: Theme.darkMode
    readonly property bool compactLayout: width < Theme.rightPanelBreakpoint
    readonly property real workspaceMinimumWidth: Theme.workspaceMinimumWidth
    readonly property real channelMinimumWidth: Theme.channelMinimumWidth
    readonly property real conversationMinimumWidth: Theme.conversationMinimumWidth
    readonly property var workspaces: ["BT", "DEV", "OPS"]
    readonly property var channels: ["general", "product", "support"]
    readonly property var messages: [qsTr("Mina: The three-pane shell is ready."), qsTr("Alex: Resize the window to check the compact layout.")]

    minimumWidth: 640
    minimumHeight: 480
    width: 800
    height: 600
    visible: true
    title: qsTr("BranchTalk")
    color: Theme.windowBackground

    function toggleTheme() {
        Theme.toggleMode();
    }

    SplitView {
        id: mainSplit

        anchors.fill: parent
        anchors.margins: Theme.spacingMedium
        orientation: Qt.Horizontal

        handle: Rectangle {
            implicitWidth: Theme.splitHandleWidth
            color: Theme.panelBorder
        }

        AppPanel {
            id: workspacePanel

            objectName: "workspacePanel"
            contentPadding: Theme.spacingSmall
            SplitView.minimumWidth: Theme.workspaceMinimumWidth
            SplitView.preferredWidth: Theme.workspacePreferredWidth

            Column {
                width: parent.width
                spacing: Theme.spacingSmall

                Text {
                    width: parent.width
                    text: qsTr("Workspaces")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.bodyFontSize
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }

                AppButton {
                    objectName: "workspaceAction"
                    width: parent.width
                    compact: true
                    text: window.workspaces[0]
                }

                Repeater {
                    model: window.workspaces.slice(1)

                    AppButton {
                        required property string modelData

                        width: parent.width
                        compact: true
                        text: modelData
                    }
                }
            }
        }

        AppPanel {
            id: channelPanel

            objectName: "channelPanel"
            contentPadding: Theme.spacingMedium
            SplitView.minimumWidth: Theme.channelMinimumWidth
            SplitView.preferredWidth: Theme.channelPreferredWidth
            SplitView.fillWidth: window.compactLayout

            Column {
                width: parent.width
                spacing: Theme.spacingSmall

                Text {
                    width: parent.width
                    text: qsTr("Channels")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.titleFontSize
                    font.weight: Theme.titleFontWeight
                    elide: Text.ElideRight
                }

                AppButton {
                    objectName: "channelAction"
                    width: parent.width
                    text: "# " + window.channels[0]
                }

                Repeater {
                    model: window.channels.slice(1)

                    AppButton {
                        required property string modelData

                        width: parent.width
                        text: "# " + modelData
                    }
                }

                AppButton {
                    objectName: "themeToggle"
                    width: parent.width
                    text: Theme.darkMode ? qsTr("Use light theme") : qsTr("Use dark theme")
                    onClicked: window.toggleTheme()
                }
            }
        }

        AppPanel {
            id: conversationPanel

            objectName: "conversationPanel"
            visible: !window.compactLayout
            SplitView.minimumWidth: Theme.conversationMinimumWidth
            SplitView.fillWidth: !window.compactLayout

            Column {
                width: parent.width
                spacing: Theme.spacingMedium

                Text {
                    width: parent.width
                    text: qsTr("# general")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.titleFontSize
                    font.weight: Theme.titleFontWeight
                }

                Repeater {
                    model: window.messages

                    Text {
                        required property string modelData

                        width: parent.width
                        text: modelData
                        color: Theme.textSecondary
                        font.pixelSize: Theme.bodyFontSize
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
