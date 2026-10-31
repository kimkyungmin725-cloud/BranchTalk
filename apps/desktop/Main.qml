import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window

    required property AppViewModel appViewModel
    required property WorkspaceListModel workspaceModel
    required property ChannelListModel channelModel

    objectName: "mainWindow"
    readonly property bool darkMode: Theme.darkMode
    readonly property bool compactLayout: width < Theme.rightPanelBreakpoint
    readonly property real workspaceMinimumWidth: Theme.workspaceMinimumWidth
    readonly property real channelMinimumWidth: Theme.channelMinimumWidth
    readonly property real conversationMinimumWidth: Theme.conversationMinimumWidth
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
                property bool channelsCollapsed: false
                contentPadding: Theme.spacingMedium
                SplitView.minimumWidth: Theme.channelMinimumWidth
                SplitView.preferredWidth: Theme.channelPreferredWidth
                SplitView.fillWidth: window.compactLayout

                AppButton {
                    id: channelCollapseAction

                    objectName: "channelCollapseAction"
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    compact: true
                    text: channelPanel.channelsCollapsed ? qsTr("Channels ▸") : qsTr("Channels ▾")
                    onClicked: channelPanel.channelsCollapsed = !channelPanel.channelsCollapsed
                }

                TextField {
                    id: channelSearch

                    objectName: "channelSearch"
                    anchors.top: channelCollapseAction.bottom
                    anchors.topMargin: Theme.spacingSmall
                    anchors.left: parent.left
                    anchors.right: parent.right
                    visible: !channelPanel.channelsCollapsed
                    height: Theme.controlHeight
                    leftPadding: Theme.spacingSmall
                    rightPadding: Theme.spacingSmall
                    text: window.channelModel.searchQuery
                    placeholderText: qsTr("Search channels")
                    color: Theme.textPrimary
                    placeholderTextColor: Theme.textSecondary
                    selectionColor: Theme.accent
                    selectedTextColor: Theme.buttonText
                    font.pixelSize: Theme.bodyFontSize
                    onTextChanged: window.channelModel.setSearchQuery(text)

                    background: Rectangle {
                        color: Theme.panelBackground
                        border.color: Theme.panelBorder
                        border.width: Theme.borderWidth
                        radius: Theme.buttonRadius
                    }
                }

                Item {
                    id: channelContent

                    objectName: "channelContent"
                    anchors.top: channelSearch.bottom
                    anchors.topMargin: Theme.spacingSmall
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: openSettingsAction.top
                    anchors.bottomMargin: Theme.spacingSmall
                    visible: !channelPanel.channelsCollapsed

                    Text {
                        id: channelNoMatchesState

                        objectName: "channelNoMatchesState"
                        anchors.centerIn: parent
                        width: parent.width
                        visible: window.channelModel.matchingChannelCount === 0
                        text: qsTr("No channels found")
                        color: Theme.textSecondary
                        font.pixelSize: Theme.bodyFontSize
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    ListView {
                        id: channelList

                        objectName: "channelList"
                        anchors.fill: parent
                        visible: window.channelModel.matchingChannelCount > 0
                        clip: true
                        spacing: Theme.spacingSmall
                        model: window.channelModel

                        delegate: AppButton {
                            required property string stableId
                            required property string name
                            required property int kind
                            required property bool selected
                            required property int unreadCount
                            required property bool muted
                            required property bool matchesSearch

                            objectName: "channelAction-" + stableId
                            width: ListView.view.width
                            height: visible ? implicitHeight : 0
                            visible: matchesSearch
                            compact: true
                            active: selected
                            text: (kind === ChannelListModel.TextChannel ? "# " : "◉ ") + name + (unreadCount > 0 ? " · " + unreadCount : "") + (muted ? qsTr(" · muted") : "")
                            onClicked: window.channelModel.selectChannel(stableId)
                        }
                    }
                }

                AppButton {
                    id: openSettingsAction

                    objectName: "openSettingsAction"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: themeToggle.top
                    anchors.bottomMargin: Theme.spacingSmall
                    text: qsTr("Settings")
                    onClicked: window.appViewModel.showSettings()
                }

                AppButton {
                    id: themeToggle

                    objectName: "themeToggle"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    text: Theme.darkMode ? qsTr("Use light theme") : qsTr("Use dark theme")
                    onClicked: window.toggleTheme()
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
                        id: conversationTitle

                        objectName: "conversationTitle"
                        width: parent.width
                        text: window.channelModel.selectedChannelName.length > 0 ? (window.channelModel.selectedChannelKind === ChannelListModel.TextChannel ? "# " : "◉ ") + window.channelModel.selectedChannelName : qsTr("No channel selected")
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
