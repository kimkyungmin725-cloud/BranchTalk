import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window

    required property AppViewModel appViewModel
    required property WorkspaceListModel workspaceModel

    objectName: "mainWindow"
    readonly property bool darkMode: Theme.darkMode
    readonly property bool compactLayout: width < Theme.rightPanelBreakpoint
    readonly property real workspaceMinimumWidth: Theme.workspaceMinimumWidth
    readonly property real channelMinimumWidth: Theme.channelMinimumWidth
    readonly property real conversationMinimumWidth: Theme.conversationMinimumWidth
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

    Item {
        id: loginView

        objectName: "loginView"
        anchors.fill: parent
        visible: window.appViewModel.currentScreen === AppViewModel.LoginScreen

        AppPanel {
            anchors.centerIn: parent
            width: Math.min(implicitWidth, parent.width - Theme.spacingLarge * 2)

            Column {
                width: Theme.channelPreferredWidth
                spacing: Theme.spacingMedium

                Text {
                    width: parent.width
                    text: qsTr("Sign in")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.titleFontSize
                    font.weight: Theme.titleFontWeight
                }

                Text {
                    width: parent.width
                    text: qsTr("Open the workspace shell.")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.bodyFontSize
                    wrapMode: Text.WordWrap
                }

                AppButton {
                    objectName: "enterMainAction"
                    width: parent.width
                    text: qsTr("Open main view")
                    onClicked: window.appViewModel.showMain()
                }
            }
        }
    }

    Item {
        id: mainView

        objectName: "mainView"
        anchors.fill: parent
        visible: window.appViewModel.currentScreen === AppViewModel.MainScreen

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

                Text {
                    id: workspaceTitle

                    anchors.top: parent.top
                    width: parent.width
                    text: qsTr("Workspaces")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.bodyFontSize
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }

                Item {
                    anchors.top: workspaceTitle.bottom
                    anchors.topMargin: Theme.spacingSmall
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    Text {
                        id: workspaceLoadingState

                        objectName: "workspaceLoadingState"
                        anchors.centerIn: parent
                        width: parent.width
                        visible: window.workspaceModel.loading
                        text: qsTr("Loading workspaces…")
                        color: Theme.textSecondary
                        font.pixelSize: Theme.bodyFontSize
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        id: workspaceEmptyState

                        objectName: "workspaceEmptyState"
                        anchors.centerIn: parent
                        width: parent.width
                        visible: !window.workspaceModel.loading && window.workspaceModel.empty
                        text: qsTr("No workspaces")
                        color: Theme.textSecondary
                        font.pixelSize: Theme.bodyFontSize
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    ListView {
                        id: workspaceList

                        objectName: "workspaceList"
                        anchors.fill: parent
                        visible: !window.workspaceModel.loading && !window.workspaceModel.empty
                        clip: true
                        spacing: Theme.spacingSmall
                        model: window.workspaceModel

                        delegate: AppButton {
                            required property string stableId
                            required property string name
                            required property bool selected
                            required property int unreadCount

                            objectName: "workspaceAction-" + stableId
                            width: ListView.view.width
                            compact: true
                            active: selected
                            text: unreadCount > 0 ? name + " · " + unreadCount : name
                            onClicked: window.workspaceModel.selectWorkspace(stableId)
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
                        objectName: "openSettingsAction"
                        width: parent.width
                        text: qsTr("Settings")
                        onClicked: window.appViewModel.showSettings()
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

    Item {
        id: settingsView

        objectName: "settingsView"
        anchors.fill: parent
        visible: window.appViewModel.currentScreen === AppViewModel.SettingsScreen

        AppPanel {
            anchors.centerIn: parent
            width: Math.min(implicitWidth, parent.width - Theme.spacingLarge * 2)

            Column {
                width: Theme.channelPreferredWidth
                spacing: Theme.spacingMedium

                Text {
                    width: parent.width
                    text: qsTr("Settings")
                    color: Theme.textPrimary
                    font.pixelSize: Theme.titleFontSize
                    font.weight: Theme.titleFontWeight
                }

                Text {
                    width: parent.width
                    text: qsTr("Choose where to go next.")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.bodyFontSize
                    wrapMode: Text.WordWrap
                }

                AppButton {
                    objectName: "settingsBackAction"
                    width: parent.width
                    text: qsTr("Back to main")
                    onClicked: window.appViewModel.showMain()
                }

                AppButton {
                    objectName: "returnToLoginAction"
                    width: parent.width
                    text: qsTr("Return to sign in")
                    onClicked: window.appViewModel.showLogin()
                }
            }
        }
    }
}
