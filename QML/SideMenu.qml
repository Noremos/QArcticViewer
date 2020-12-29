import QtQuick 2.15
import QtQuick.Window 2.15
import Qt3D.Core 2.14
import Qt3D.Render 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    //    color: "#d4d1d1"
    enum Side {
        Top,
        Left,
        Right,
        Bottom
    }
    //    property var sideItem: null
    property int hei: 50
    property alias color: root.color

    ColumnLayout {
        spacing: 5
        //        anchors.fill: parent
        Label {
            text: "Кооэфициент отношения"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        SpinFloat {
            id: coofSB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            realFrom: 1.0
            realValue: 1.7
            realTo: 5.0
            onValueChanged: updateBoxSetts()
        }
        Label {
            text: "Минимальный диаметр"
            Layout.topMargin: 7
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        SpinBox {
            id: dminSB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            from: 1
            value: 10
            to: dmaxDB.value
            editable: true
            onValueChanged: updateBoxSetts()
        }

        Label {
            text: "Максимальный диаметр"
            Layout.topMargin: 7
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        SpinBox {
            id: dmaxDB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            from: dminSB.value
            value: 300
            to: 1000
            editable: true
            onValueChanged: updateBoxSetts()
        }

        Label {
            text: "Минимальная восота"
            Layout.topMargin: 7
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        SpinFloat {
            id: minSizeHeiSB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            realFrom: 0.0
            realValue: 2
            to: maxSizeHeiSB.value
            onValueChanged: updateBoxSetts()
        }
        Label {
            text: "Максимальная восота"
            Layout.topMargin: 7
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        SpinFloat {
            id: maxSizeHeiSB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            from: minSizeHeiSB.value
            realValue: 6
            realTo: 100.0
            onValueChanged: updateBoxSetts()
        }
        Label {
            text: "Процент отсечения снизу"
            Layout.topMargin: 7
        }
        SpinBox {
            id: bottomLenSB
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            from: 0
            value: 10
            to: 100
            onValueChanged: updateBoxSetts()
        }
        Button {
            text: "Поиск"
            onClicked: backend.findByParams()
        }

        //                coof
        //                dmin
        //                dmin
        //                sizeTop (min max
        //                boottom line
    }

    //    property int side: SideMenu.Side.Top
    //    onSideChanged: {
    //        switch (root.side) {
    //        case SideMenu.Side.Top:
    //            anchors.left = sideItem.left
    //            anchors.leftMargin = 0

    //            anchors.right = sideItem.right
    //            anchors.rightMargin = 0

    //            anchors.top = sideItem.top
    //            anchors.topMargin = 0

    //            height = hei
    //            break
    //        case SideMenu.Side.Botton:
    //            anchors.left = sideItem.left
    //            anchors.leftMargin = 0

    //            anchors.right = sideItem.right
    //            anchors.rightMargin = 0

    //            anchors.bottom = sideItem.bottom
    //            anchors.bottomMargin = 0

    //            height = hei
    //            break
    //        case SideMenu.Side.Left:
    //            anchors.top = sideItem.top
    //            anchors.topMargin = 0

    //            anchors.bottom = sideItem.bottom
    //            anchors.bottomMargin = 0

    //            anchors.left = sideItem.left
    //            anchors.leftMargin = 0

    //            width = hei
    //            break
    //        case SideMenu.Side.Right:
    //            anchors.top = sideItem.top
    //            anchors.topMargin = 0

    //            anchors.bottom = sideItem.bottom
    //            anchors.bottomMargin = 0

    //            anchors.right = sideItem.left
    //            anchors.rightMargin = 0

    //            width = hei
    //            break
    //        }
    //    }
    function getVal(sbox) {
        return sbox.value / sbox.factor
    }

    function setBoxSetts() {

        coofSB.realValue = searchSettings.coof
        dminSB.value = searchSettings.diamertMin
        dmaxDB.value = searchSettings.diamertMax
        minSizeHeiSB.realValue = searchSettings.heightMin
        maxSizeHeiSB.realValue = searchSettings.heightMax
        bottomLenSB.value = searchSettings.bottomProc
    }

    function updateBoxSetts() {
        if (backend.block)
            return

        searchSettings.coof = getVal(coofSB)
        searchSettings.diamertMin = dminSB.value
        searchSettings.diamertMax = dmaxDB.value
        searchSettings.heightMin = getVal(minSizeHeiSB)
        searchSettings.heightMax = getVal(maxSizeHeiSB)
        searchSettings.bottomProc = bottomLenSB.value
        backend.setSearchingsettings()
        //        getVal(coofSB), dminSB.value,
        //                                     dmaxDB.value, getVal(minSizeHeiSB),
        //                                     getVal(maxSizeHeiSB), bottomLenSB.value)
        //        console.log(coofSB.value)
        //        console.log(getVal(coofSB))
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

