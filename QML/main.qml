import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtQuick.Scene3D 2.15
import My 1.0
import "."

ApplicationWindow {
    id: applicationWindow
    width: 1280
    height: 720
    visible: true
    title: qsTr("Hello World")

    property alias spotZones: surf.spotZones
    property alias markerZones: surf.markerZones
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
        height: 100
        Row {
            anchors.fill: parent
            LoadButton {
                id: fileDialog
                onAccepted: {
                    filePath = backend.loadImage(filePath.toString().substring(
                                                     8), simpithithion.value,
                                                 proiz.checked ? 0 : 1)

                    surf.setModelSource("file:///" + filePath)
                }
            }
            SpinBox {
                id: simpithithion
                from: 1
                value: 10
                to: 1000
                editable: true
            }

            ColumnLayout {
                RadioButton {
                    id: proiz
                    checked: true
                    text: qsTr("Производительность")
                }
                RadioButton {
                    text: qsTr("Скорость")
                }
            }
            CheckBox {
                id: useText
                text: "Использовать текстуру"
                checked: false
                onCheckedChanged: surf.setDrawingMode(checked)
            }

            LoadButton {
                id: textureLoder
                onAccepted: {

                    surf.setTextureSource(filePath)
                }
            }
            Button {
                id: findZones
                onClicked: {

                    backend.findZones(simpithithion.value)
                    surf.update()
                    surf.ude()
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
    Rectangle {

        id: mainZone

        anchors.left: leftmenu.right
        anchors.right: parent.right
        anchors.top: topmenu.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        color: "gray"

        SurfaceScene {
            id: surf
            objectName: "surf"
            anchors.fill: parent
            onFocusChanged: {
                if (focus === false) {
                    focusArea.enabled = true
                    //                    mainZone.chi
                }
            }
        }
        MouseArea {
            id: focusArea
            enabled: false
            anchors.fill: parent
            //            preventStealing: false
            //            propagateComposedEvents: false
            onClicked: {
                surf.focus = true
                enabled = false
            }
        }
    }
    ProgressBar {
        anchors.left: leftmenu.right
        anchors.right: parent.right
        anchors.top: topmenu.bottom
        value: 0
        from: 0
        to: 100
        visible: false
    }
}
