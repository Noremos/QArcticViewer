import QtQuick 2.15
import QtQuick.Window 2.15
import Qt3D.Core 2.14
import Qt3D.Render 2.15
import QtQuick.Controls 2.15

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
    Row {
        anchors.fill: parent
    }
}
