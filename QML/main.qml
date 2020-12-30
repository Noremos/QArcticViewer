import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtQuick.Scene3D 2.15
import My 1.0
import "."

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: qsTr("Hello World")
    visibility: "Maximized"
    onClosing: {
        console.log(projectParams.texturePath)
        backend.saveSettings()
    }

    property alias spotZones: surf.spotZones
    property alias markerZones: surf.markerZones

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
        height: 120
        Row {
            spacing: 5
            //            anchors.fill: parent
            ColumnLayout {
                LoadButton {
                    id: fileDialog
                    onAccepted: {
                        filePath = backend.loadImage(filePath.toString(
                                                         ).substring(8),
                                                     simpithithion.value, 0)
                    }
                }
                SpinBox {
                    id: simpithithion
                    from: 1
                    value: 10
                    to: 1000
                    editable: true
                }
            }

            //            ColumnLayout {
            //                RadioButton {
            //                    id: proiz
            //                    checked: true
            //                    text: qsTr("Производительность")
            //                }
            //                RadioButton {
            //                    text: qsTr("Скорость")
            //                }
            //            }
            ColumnLayout {
                Label {
                    text: "Что отображать на карте"
                }

                SpinBox {
                    id: mattype
                    from: 0
                    to: items.length - 1
                    value: projectParams ? projectParams.materialType : 0 // "Высота"

                    property var items: ["Высота", "Свет", "Текстура 1", "Тексутра 2"]

                    validator: RegExpValidator {
                        regExp: new RegExp("(Высота|Текстура 1|Свет|Тексутра 2)",
                                           "i")
                    }

                    textFromValue: function (value) {
                        return items[value]
                    }

                    valueFromText: function (text) {
                        for (var i = 0; i < items.length; ++i) {
                            if (items[i].toLowerCase().indexOf(
                                        text.toLowerCase()) === 0)
                                return i
                        }
                        return sb.value
                    }
                    onValueChanged: {
                        if (projectParams)
                        {
                                  console.log("check")
                            projectParams.materialType = mattype.value
                            surf.setTexturetype()
                        }
                    }
                }

                LoadButton {
                    id: textureLoder
                    text: "Загрузить текстуру"
                    enabled: mattype.value > 1
                    onAccepted: {
                        if (!projectParams)
                            return

                        if (mattype.value == 2)
                            projectParams.texturePath = filePath
                        else if (mattype.value == 3)
                            projectParams.texture2Path = filePath
                        surf.setTexturetype()
                    }
                }
            }
            ColumnLayout {
                Button {
                    id: findZones
                    text: "Построить баркоды"
                    onClicked: {
                        backend.processHiemap(startProc.value, endProc.value)
                        //                        surf.update()
                        //                        surf.ude()
                    }
                }
                CheckBox {
                    text: "Показать найденные"
                    checked: true
                    onCheckedChanged: surf.setFindingMode(checked)
                }
            }
            ColumnLayout {
                Label {
                    text: "Начать с"
                }

                SpinBox {
                    id: startProc
                    from: 0
                    value: 0
                    to: 25000
                    stepSize: 1000
                    editable: true
                }
                SpinBox {
                    id: endProc
                    from: 0
                    stepSize: 1000

                    value: 25000
                    to: 25000
                    editable: true
                }
            }
            ColumnLayout {
                Label {
                    text: "Множитель выосты"
                }
                SpinBox {
                    id: heightSpin
                    objectName: "factor"
                    from: -10000
                    value: 10
                    to: 1000000
                    editable: true
                }
            }
            Button {
                text: qsTr("Сохранить изменения")
                onClicked: backend.saveSettings()
            }
        }
    }
    SideMenu {
        id: leftmenu
        objectName: "sideMenu"
        color: "#d4d1d1"

        //        side: SideMenu.Side.Left
        anchors.left: parent.left
        anchors.leftMargin: 0

        anchors.top: topmenu.bottom
        anchors.topMargin: 0

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        width: 150
        //        Column {
        //            anchors.fill: parent
        //        }
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
            surfaceFactor: heightSpin.value
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
