import QtQuick

Rectangle {
    id: panel

    default property alias contentData: panelContent.data
    readonly property alias contentItem: panelContent
    property int contentPadding: Theme.spacingLarge

    implicitWidth: panelContent.implicitWidth + contentPadding * 2
    implicitHeight: panelContent.implicitHeight + contentPadding * 2
    color: Theme.panelBackground
    border.color: Theme.panelBorder
    border.width: Theme.borderWidth
    radius: Theme.panelRadius

    Item {
        id: panelContent

        anchors.fill: parent
        anchors.margins: panel.contentPadding
        implicitWidth: childrenRect.width
        implicitHeight: childrenRect.height
    }
}
