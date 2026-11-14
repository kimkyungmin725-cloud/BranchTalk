import QtQuick

Rectangle {
    id: panel

    default property alias contentData: panelContent.data
    readonly property alias contentItem: panelContent
    property int contentPadding: Theme.spacingLarge
    property real contentImplicitWidth: 0
    property real contentImplicitHeight: 0

    implicitWidth: contentImplicitWidth + contentPadding * 2
    implicitHeight: contentImplicitHeight + contentPadding * 2
    color: Theme.panelBackground
    border.color: Theme.panelBorder
    border.width: Theme.borderWidth
    radius: Theme.panelRadius

    Item {
        id: panelContent

        anchors.fill: parent
        anchors.margins: panel.contentPadding
    }
}
