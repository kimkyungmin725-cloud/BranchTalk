import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window

    objectName: "mainWindow"
    readonly property bool darkMode: Theme.darkMode

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

    AppPanel {
        objectName: "mainPanel"
        anchors.centerIn: parent
        width: Math.min(Theme.contentWidth, parent.width - Theme.spacingLarge * 2)

        Column {
            width: parent.width
            spacing: Theme.spacingMedium

            Text {
                width: parent.width
                text: qsTr("BranchTalk")
                color: Theme.textPrimary
                font.pixelSize: Theme.titleFontSize
                font.weight: Theme.titleFontWeight
            }

            Text {
                width: parent.width
                text: qsTr("Colors, spacing, and typography now share one source of truth.")
                color: Theme.textSecondary
                font.pixelSize: Theme.bodyFontSize
                wrapMode: Text.WordWrap
            }

            AppButton {
                objectName: "themeToggle"
                text: Theme.darkMode ? qsTr("Use light theme") : qsTr("Use dark theme")
                onClicked: window.toggleTheme()
            }
        }
    }
}
