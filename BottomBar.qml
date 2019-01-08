import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

/* The bottom bar
*/

Rectangle {
    id: root

    signal saveClicked

    height: 50

    RowLayout {
        id: layout
        anchors.fill: parent
        spacing: 10

        Button {
            id: saveMaskBtn
            text: "Save Mask"
            Layout.maximumWidth: 100
            height: root.height
            font.pixelSize: 12

            onClicked: {
                root.saveClicked();
            }
        }


    }
}
