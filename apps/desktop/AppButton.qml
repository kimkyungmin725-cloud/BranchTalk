import QtQuick
import QtQuick.Controls

Button {
    id: control

    property bool compact: false

    implicitWidth: Math.max(compact ? Theme.controlHeight : Theme.buttonMinimumWidth, contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Theme.controlHeight
    leftPadding: compact ? Theme.spacingSmall : Theme.spacingLarge
    rightPadding: compact ? Theme.spacingSmall : Theme.spacingLarge
    topPadding: Theme.spacingSmall
    bottomPadding: Theme.spacingSmall
    font.pixelSize: Theme.buttonFontSize
    font.weight: Theme.buttonFontWeight

    contentItem: Text {
        text: control.text
        color: control.enabled ? Theme.buttonText : Theme.disabledText
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: !control.enabled ? Theme.disabledBackground : control.down ? Theme.accentPressed : control.hovered ? Theme.accentHovered : Theme.accent
        radius: Theme.buttonRadius
    }
}
