import QtQuick 2.9
import QtQuick.Controls 2.0

import CppBackend 1.0

ApplicationWindow {
    id: root

    property SelectionModel selectionModel: backendModel
    property FileManager fileManager: backendManager

    visible: true

    width: 1200
    height: 900
    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width

    title: qsTr("Masquer")

    color: "darkgrey"

    MainBar {
        id: menu

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        infoText: drawer.statusText
        currentImageIndex: fileManager.currentImageNumber
        totalImages: fileManager.totalImages

        onFileSelected: {
            fileManager.openFile(path);
        }
        onFolderSelected: {
            fileManager.openDir(path);
        }
        onForwardClicked: {
            fileManager.loadNextImage();
        }
        onBackwardClicked: {
            fileManager.loadPrevImage();
        }
        onIndexEntered: {
            fileManager.loadImageByIndex(idx);
        }
    }

    MaskDrawer {
        id: drawer

        anchors.left: parent.left
        anchors.top: menu.bottom
        anchors.right: parent.right
        anchors.bottom: bottomBar.top

        model: root.selectionModel
        manager: root.fileManager
    }

    BottomBar {
        id: bottomBar

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        maskSubfolderPath: fileManager.maskSubfolderRelPath

        onSaveClicked: {
            drawer.saveMask();
        }

        onMaskSubdirEntered: {
            fileManager.setMaskSubfolderRelPath(path);
        }
    }
}
