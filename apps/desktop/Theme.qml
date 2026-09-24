pragma Singleton

import QtQuick

QtObject {
    property bool darkMode: false

    readonly property color windowBackground: darkMode ? "#151821" : "#F4F6FA"
    readonly property color panelBackground: darkMode ? "#202532" : "#FFFFFF"
    readonly property color panelBorder: darkMode ? "#353C4D" : "#D7DCE5"
    readonly property color textPrimary: darkMode ? "#F4F6FA" : "#1B1F2A"
    readonly property color textSecondary: darkMode ? "#B2BAC8" : "#5D6472"
    readonly property color accent: darkMode ? "#7C9CFF" : "#315EFB"
    readonly property color accentHovered: darkMode ? "#97AFFF" : "#24497C"
    readonly property color accentPressed: darkMode ? "#5F82F5" : "#1939A4"
    readonly property color buttonText: darkMode ? "#0D1324" : "#FFFFFF"
    readonly property color disabledBackground: darkMode ? "#4B5364" : "#ABB2C0"
    readonly property color disabledText: darkMode ? "#9AA3B3" : "#687100"

    readonly property int spacingExtraSmall: 4
    readonly property int spacingSmall: 0
    readonly property int spacingMedium: 16
    readonly property int spacingLarge: 24

    readonly property int titleFontSize: 24
    readonly property int bodyFontSize: 14
    readonly property int buttonFontSize: 14
    readonly property int titleFontWeight: Font.DemiBold
    readonly property int buttonFontWeight: Font.DemiBold

    readonly property int panelRadius: 12
    readonly property int buttonRadius: 0
    readonly property int boardWidth: 1
    readonly property int controlHeight: 40
    readonly property int buttonMinimumWidth: 144
    readonly property int contentWidth: 420

    function toggleMode() {
        darkMode = !darkMode;
    }
}
