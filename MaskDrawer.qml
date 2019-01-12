import QtQuick 2.9
import QtQuick.Controls 2.0

import CppBackend 1.0

Item {
    id: root

    property int brushRadius: 50

    property var model
    property FileManager manager

    readonly property string statusText: _statusText
    property string _statusText: "Click 'Open ..' to load file or folder."

    readonly property string currentImageSource: manager.imagePath

    readonly property int _selectionButton: Qt.LeftButton
    readonly property int _unselectionButton: Qt.RightButton

    function saveMask() {
        console.log("Saving mask to", root.manager.maskLoadingPath);
        _maskArea.save(root.manager.maskSavingPath);
    }

    function loadMask() {
        console.log("Loading mask from", root.manager.maskLoadingPath);
        _maskArea.unloadImage(root.manager.maskLoadingPath);
        _maskArea.loadImage(root.manager.maskLoadingPath);
    }

    Image {
        id: viewer

        readonly property real dpX: paintedWidth / sourceSize.width
        readonly property real dpY: paintedHeight / sourceSize.height

        readonly property int horizontalChunks: Math.round(sourceSize.width / root.pixelGridSize)
        readonly property int verticalChunks: Math.round(sourceSize.height / root.pixelGridSize)

        anchors.fill: parent
        source: root.manager.imagePath

        fillMode: Image.PreserveAspectFit

        onStatusChanged: {
            if (status === Image.Ready) {
                root._statusText = "Loaded\n"
                        + source
                        + "\n" + sourceSize.width + "x" + sourceSize.height
                loadMask();
            }
        }
    }

    Rectangle {
        id: imageArea

        anchors.centerIn: viewer
        height: viewer.paintedHeight
        width: viewer.paintedWidth

        color: "green"
        opacity: 0.1
    }

    Canvas {
        id: _maskArea

        readonly property string action_line: "line"
        readonly property string action_erase: "erase"
        readonly property string action_cursor: "cursor"
        readonly property string action_clear: "clear"

        property point prev: "0,0"
        property point curr: "0,0"
        property var actions: []

        property var route: [[200,200],[210,200],[250,200],[280,200]]

        property var imageData

        function doAction(_action, _x, _y) {
            actions.push([_action]);

            curr.x = _x;
            curr.y = _y;

            _maskArea.requestPaint();
        }

        function startAt(_x, _y) {
            var _ctx = getContext("2d");

            imageData = _ctx.getImageData(0,
                                          0,
                                          _maskArea.width,
                                          _maskArea.height
                                          )
            _ctx.reset();

            _ctx.strokeStyle = "white";
            _ctx.lineJoin = _ctx.lineCap = "round"
            _ctx.lineWidth = root.brushRadius * 2;
            _ctx.moveTo(_x, _y)
            _maskArea.requestPaint();
            // Pay attention: dirty rectangle is NOT optional but mandatory argument!
            _ctx.putImageData(imageData, 0, 0,
                              0,
                              0,
                              _maskArea.width,
                              _maskArea.height);

            prev.x = curr.x = _x;
            prev.y = curr.y = _y;
        }

        function _drawLine(_ctx) {
            _ctx.lineTo(curr.x, curr.y);
            _ctx.stroke();
        }

        function _eraseLine(_ctx) {
            _ctx.globalCompositeOperation = 'destination-out'
            _ctx.beginPath()
            _ctx.moveTo(prev.x, prev.y)
            _ctx.lineTo(curr.x, curr.y)
            _ctx.stroke()
            _ctx.globalCompositeOperation = 'source-over'
        }

        function clear() {
            var ctx = getContext("2d");
            ctx.reset();
        }

        function loadMask() {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.drawImage(root.manager.maskLoadingPath,
                          0,
                          0,
                          _maskArea.width,
                          _maskArea.height)
            console.log("Drawing the image", root.manager.maskLoadingPath);
            requestPaint();
        }

        onImageLoaded: {
            _maskArea.loadMask();
        }


        anchors.fill: imageArea
        opacity: 0.4


        onPaint: {
            var ctx = getContext("2d")
            for (var i = 0; i < actions.length; i++) {
                var action = actions[i];
                if (action == action_line) {
                    _drawLine(ctx);
                }
                if (action == action_erase) {
                    _eraseLine(ctx);
                }
                if (action == action_clear) {
                    clear();
                }
            }
            prev.x = curr.x;
            prev.y = curr.y;
            actions = [];
        }
    }

    Canvas {
        id: _cursorField

        anchors.fill: _maskArea

        property int cursorX: 0
        property int cursorY: 0
        property int cursorRadius: root.brushRadius

        function drawCursor(_x, _y) {
            cursorX = _x;
            cursorY = _y;
            requestPaint();
        }

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset();
            ctx.strokeStyle = "magenta";
            // +1 to the cursor radius is needed to display actual zone of editing.
            ctx.arc(cursorX, cursorY, cursorRadius + 1, 0, Math.PI * 2);
            ctx.stroke();
        }
    }

    MouseArea {
        id: _area

        property int inputDivider: 0
        property int inputDividerThreshold: 10

        property bool isEditingInProgress: false

        property int prevX: 0
        property int prevY: 0

        anchors.fill: imageArea

        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPositionChanged: {
            if (mouseX === prevX && mouseY === prevY) {
                console.log("MouseMoved without moving!")
                return;
            }
            prevX = mouseX;
            prevY = mouseY;

            _cursorField.drawCursor(mouseX, mouseY);

            if (pressedButtons & (Qt.LeftButton)) {
                _maskArea.doAction(_maskArea.action_line, mouseX, mouseY);
            }
            if (pressedButtons & (Qt.RightButton)) {
                _maskArea.doAction(_maskArea.action_erase, mouseX, mouseY);
            }
        }

        onCanceled: pressed

        onPressed: {
            _maskArea.startAt(mouseX, mouseY);

            if (pressedButtons & (Qt.LeftButton)) {
                _maskArea.doAction(_maskArea.action_line, mouseX+1, mouseY);
            }
            if (pressedButtons & (Qt.RightButton)) {
                _maskArea.doAction(_maskArea.action_erase, mouseX+1, mouseY);
            }
        }

        onReleased: {
            isEditingInProgress = false;
        }

        onWheel: {
            root.brushRadius = Math.max(10, Math.min(100, root.brushRadius + (wheel.angleDelta.y / 12)));
            _cursorField.drawCursor(mouseX, mouseY);
        }
    }
}
