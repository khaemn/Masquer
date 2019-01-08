import QtQuick 2.9

Rectangle {
    id: root
    
    property bool selected: false
    property bool unused: false

    readonly property color _selectedColor: "#4000FF00"
    readonly property color _unusedColor: "#40FF0000"
    readonly property color _defaultColor:  "#00000000"
    readonly property color _hoveredBorderColor: "yellow"
    readonly property color _borderColor: "grey"

    readonly property int _selectionButton: Qt.LeftButton
    readonly property int _unselectionButton: Qt.RightButton

    signal triggered(bool wasSelected)

    border.color: _borderColor
    border.width: 1

    width: 100
    height: 100

    color: selected
           ? _selectedColor
           : unused
                ? _unusedColor
                : _defaultColor

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            root.border.color = _hoveredBorderColor;
        }
        onExited: {
            root.border.color = _borderColor
        }
    }
}
