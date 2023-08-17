import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import AsemanQml.Base 2.0

Button {
    id: root

    property color color: "white"
    /// 背景颜色
    property color backgroundColor: "#4E5BF2"
    property color backgroundPressedColor: Qt.darker(backgroundColor, 1.2)

    property string fontFamily: ""
    property int fontSize: 10

    property bool showBackground: false
    property int horizontalAlignment: Text.AlignHCenter
    property int verticalAlignment: Text.AlignVCenter

    text: "Button"
    flat : true
    contentItem: Text {
        text: root.text
        color: root.color
        font.family: fontFamily
        font.pixelSize: Math.floor(fontSize*Devices.fontDensity)
        horizontalAlignment: root.horizontalAlignment
        verticalAlignment: root.verticalAlignment
        width: root.width
        height: root.height
    }
//    background: Rectangle {
//        implicitWidth: 83
//        implicitHeight: 37
//        color: root.down ? root.backgroundPressedColor : root.backgroundColor
//        radius: 3
//        layer.enabled: !flat
//        layer.effect: DropShadow {
//            transparentBorder: true
//            color: root.down ? root.backgroundPressedColor : root.backgroundColor
//            samples: 20
//        }
//    }
}
