import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

/* The main tool bar allows user to select pixel grid size,
    open a desired folder with images, navigate <> in the selected folder.
    When a <- or -> is clicked, the corresponding cignal is emitted to main
    window, the same when the new grid is selected or user wants to select directory.
*/

Rectangle {
    id: root

    property string infoText: "information text"
    property int currentImageIndex
    property int totalImages

    readonly property var pixelGridSizes: [500, 200, 100, 50, 20, 10]
    readonly property int selectedPixelGridSize: _pixelGridSize

    property int _pixelGridSize: 100

    signal forwardClicked
    signal backwardClicked
    signal folderSelected(string path)
    signal fileSelected(string path)
    signal indexEntered(int idx)

    height: 50

    RowLayout {
        id: layout
        anchors.fill: parent
        spacing: 10

        Button {
            id: openFileBtn
            text: "Open file"
            Layout.maximumWidth: 100
            height: root.height
            font.pixelSize: 12

            onClicked: {
                fileDialog.nameFilters = [ "Image files (*.jpg *.png)", "All files (*)" ];
                fileDialog.selectFolder = false;
                fileDialog.open()
            }
        }

        Button {
            id: openDirBtn
            text: "Open dir"
            Layout.maximumWidth: 100
            height: root.height
            font.pixelSize: 12

            onClicked: {
                fileDialog.selectFolder = true;
                fileDialog.open()
            }
        }

        Text {
            id: gridText
            text: "Grid chunk (px):"
            width: contentWidth
            height: root.height
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 12
        }

        ComboBox {
            id: gridSelector
            model: root.pixelGridSizes
            Layout.maximumWidth: 80
            height: root.height
            currentIndex: 2 // 100 px
            onCurrentIndexChanged: {
                root._pixelGridSize = pixelGridSizes[currentIndex];
            }
        }

        // TODO: replace by more detailed info panel
        // with info about: filepath, n/m, pixel size, and so on.
        Text {
            id: infoText
            text: root.infoText

            Layout.fillWidth: true
            Layout.minimumWidth: 300

            height: root.height
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 12
        }

        Button {
            id: backwardBtn
            text: "<"
            Layout.maximumWidth: 50
            height: root.height
            font.pixelSize: 12

            onClicked: {
                root.backwardClicked();
                _imageIndex.text = Qt.binding(function(){ return root.currentImageIndex; });
            }
        }

        TextInput {
            id: _imageIndex
            text: root.currentImageIndex
            Layout.maximumWidth: contentWidth
            inputMask: "000"
            onEditingFinished: {
                root.indexEntered(text);
            }
        }

        Text {
            id: _totalImages
            Layout.maximumWidth: 20
            text: "/ " + root.totalImages
        }

        Button {
            id: forwardBtn
            text: ">"
            Layout.maximumWidth: 50
            height: root.height
            font.pixelSize: 12

            onClicked: {
                _imageIndex.text = Qt.binding(function(){ return root.currentImageIndex; });
                root.forwardClicked();
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a folder with images or a single image"
        folder: shortcuts.home

        onAccepted: {
            console.log("User selected path: " + fileDialog.fileUrls);
            if (selectFolder) {
                root.folderSelected(fileDialog.fileUrls);
            } else {
                root.fileSelected(fileDialog.fileUrls);
            }
        }
        onRejected: {
            console.log("Folder selection anceled");
        }
    }
}
