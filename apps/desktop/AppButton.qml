import QtQuick
import QtQuick.Controls

Button {
    id: control 

    implicitWidth: Math.max(Theme.buttonMinimumWidth,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Theme.controlHeight
    leftPadding: Theme.spacingLarge
    rightPadding: Theme.spacingLarge
    topPadding: Theme.spacingSmall
    bottomPadding: Theme.spacingSmall
    font.pixelSize: Theme.buttonFontSize;
    font.weight: THeme.buttonFontWeight

    contentItem: Text {
        text: control.text
        color: control.enabled ? Theme.buttonText : Theme.disabledText
        font: control.font
        horizontalAlignment: Text.Alignment 
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: !control.enabled ? Theme.disabledBackground : control.down ? Theme.accentPressed : control.hovered ? Theme.accentHovered : Theme.accent 
        radius: Theme.buttonRadius
    }
}