import QtQuick

Rectangle {
    id: panel

    default property alias contentData: panelContent.data
    readonly property alias contentItem: panelContent

    implicitWidth: panelContent.implicitWidth + Theme.spacingLarge * 2
    implicitHeight: panelContent.implicitHeight + Theme.spacingLarge * 2
    color: Theme.panelBackground
    border.color: Theme.panelBorder
    border.width: Theme.borderWidth
    radius: Theme.panelRadius

    Item {
        id: panelContent

        anchors.fill: parent
        anchors.margins: Theme.spacingLarge
        implicitWidth: childrenRect.width
        implicitHeight: childrenRect.height
    }
}
