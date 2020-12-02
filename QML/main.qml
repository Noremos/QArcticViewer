import QtQuick 2.15
import QtQuick.Window 2.15
import Qt3D.Core 2.14
import Qt3D.Render 2.15
import QtQuick.Controls 2.15
import QtQuick.Scene3D 2.15
import My 1.0

ApplicationWindow {
    id: applicationWindow
    width: 1280
    height: 720
    visible: true
    title: qsTr("Hello World")

    //    DataImage {
    //        id: surfaceData
    //    }
    Rectangle {
        id: topmenu
        color: "#a4a4a4"

        //side: SideMenu.Side.Top
        anchors.left: parent.left
        anchors.leftMargin: 0

        anchors.right: parent.right
        anchors.rightMargin: 0

        anchors.top: parent.top
        anchors.topMargin: 0
        height: 50
        Row {
            anchors.fill: parent
            LoadButton {
                id: fileDialog
                onAccepted: {
                    mainZone.run(filePath.toString().substring(8))
                }
            }
        }
    }
    Rectangle {
        id: leftmenu
        color: "#d4d1d1"

        //        side: SideMenu.Side.Left
        anchors.left: parent.left
        anchors.leftMargin: 0

        anchors.top: topmenu.bottom
        anchors.topMargin: 0

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        width: 50
        Column {
            anchors.fill: parent
        }
    }

    HeiScene {
        id: mainZone
        anchors.left: leftmenu.right
        anchors.right: parent.right
        anchors.top: topmenu.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
    }
}
