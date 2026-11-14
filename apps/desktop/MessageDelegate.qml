import QtQuick

Item {
    id: messageDelegate

    required property string stableId
    required property string authorId
    required property string authorName
    required property string timestamp
    required property string body
    required property int status
    required property bool groupedWithPrevious

    readonly property bool headerVisible: !groupedWithPrevious
    readonly property string statusText: status === MessageListModel.Pending
                                         ? qsTr("Pending")
                                         : status === MessageListModel.Failed
                                           ? qsTr("Failed")
                                           : qsTr("Sent")

    objectName: "messageDelegate-" + stableId
    implicitHeight: messageContent.implicitHeight
                    + (groupedWithPrevious ? Theme.spacingExtraSmall
                                           : Theme.spacingMedium)
                    + Theme.spacingExtraSmall

    Column {
        id: messageContent

        anchors.top: parent.top
        anchors.topMargin: messageDelegate.groupedWithPrevious
                           ? Theme.spacingExtraSmall
                           : Theme.spacingMedium
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: Theme.spacingExtraSmall

        Row {
            id: messageHeader

            visible: messageDelegate.headerVisible
            spacing: Theme.spacingSmall

            Text {
                text: messageDelegate.authorName
                color: Theme.textPrimary
                font.pixelSize: Theme.bodyFontSize
                font.weight: Theme.buttonFontWeight
            }

            Text {
                text: messageDelegate.timestamp
                color: Theme.textSecondary
                font.pixelSize: Theme.bodyFontSize
            }
        }

        Text {
            width: parent.width
            text: messageDelegate.body
            color: Theme.textPrimary
            font.pixelSize: Theme.bodyFontSize
            wrapMode: Text.WordWrap
        }

        Text {
            width: parent.width
            text: messageDelegate.statusText
            color: Theme.textSecondary
            font.pixelSize: Theme.bodyFontSize
            horizontalAlignment: Text.AlignRight
        }
    }
}
