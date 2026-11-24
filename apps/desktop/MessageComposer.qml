import QtQuick
import QtQuick.Controls

Item {
    id: composer

    required property MessageComposerState composerState

    readonly property int characterCount: composerState.characterCount
    readonly property bool inputMethodComposing: messageInput.inputMethodComposing
    readonly property bool canSend: composerState.canSend && !inputMethodComposing
    readonly property string statusText: inputMethodComposing
                                         ? qsTr("Composing...")
                                         : canSend ? qsTr("Ready to send")
                                                   : qsTr("Enter a message")

    objectName: "messageComposer"
    implicitHeight: composerContent.implicitHeight

    Column {
        id: composerContent

        anchors.left: parent.left
        anchors.right: parent.right
        spacing: Theme.spacingSmall

        TextArea {
            id: messageInput

            objectName: "messageComposerInput"
            width: parent.width
            height: Theme.composerInputHeight
            leftPadding: Theme.spacingSmall
            rightPadding: Theme.spacingSmall
            topPadding: Theme.spacingSmall
            bottomPadding: Theme.spacingSmall
            text: composer.composerState.text
            placeholderText: qsTr("Write a message")
            color: Theme.textPrimary
            placeholderTextColor: Theme.textSecondary
            selectionColor: Theme.accent
            selectedTextColor: Theme.buttonText
            font.pixelSize: Theme.bodyFontSize
            wrapMode: TextEdit.Wrap
            activeFocusOnTab: true

            onTextChanged: {
                if (composer.composerState.text !== text) {
                    composer.composerState.text = text
                }
            }

            Keys.onPressed: event => {
                if (event.key !== Qt.Key_Return && event.key !== Qt.Key_Enter) {
                    return
                }

                const actionModifiers = event.modifiers
                        & (Qt.ShiftModifier | Qt.ControlModifier
                           | Qt.AltModifier | Qt.MetaModifier)
                if (actionModifiers === Qt.ShiftModifier) {
                    event.accepted = false
                    return
                }
                if (actionModifiers !== Qt.NoModifier) {
                    event.accepted = false
                    return
                }

                event.accepted = composer.composerState.submit(inputMethodComposing)
            }

            background: Rectangle {
                color: Theme.panelBackground
                border.color: messageInput.activeFocus ? Theme.accent : Theme.panelBorder
                border.width: Theme.borderWidth
                radius: Theme.buttonRadius
            }
        }

        Row {
            anchors.right: parent.right
            spacing: Theme.spacingSmall

            Text {
                objectName: "messageComposerStatus"
                height: sendAction.height
                text: composer.statusText
                color: Theme.textSecondary
                font.pixelSize: Theme.bodyFontSize
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                objectName: "messageComposerCharacterCount"
                height: sendAction.height
                text: qsTr("%1 characters").arg(composer.characterCount)
                color: Theme.textSecondary
                font.pixelSize: Theme.bodyFontSize
                verticalAlignment: Text.AlignVCenter
            }

            AppButton {
                id: sendAction

                objectName: "messageComposerSendAction"
                compact: true
                enabled: composer.canSend
                text: qsTr("Send")
                onClicked: composer.composerState.submit(messageInput.inputMethodComposing)
            }
        }
    }
}
