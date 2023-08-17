import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
//import QtSystemInfo 5.0
// Aseman扩展的一定要放到Qt自带的后面，相同名字的组件就会被覆盖掉
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0
import AsemanQml.MaterialIcons 2.0
import AsemanQml.Awesome 2.0
import MJPEGCapture 1.0
import ImageItem 1.0

Rectangle {
    /// 通常用root作为根的id，子对象才方便访问属性
    id:root
    width: parent.width
    height: parent.height
    property bool finished_flg: false

    signal finished()

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/splash_image.png"
    }

    Timer{
        id: timer
        interval: 1000
        repeat: false
        onTriggered: {
            console.log("移除开屏界面");
            finished_flg = true
            finished()
            root.visible = false
            root.destroy()
        }
    }

    Component.onCompleted: {
        console.log("显示开屏界面");
        timer.start()
    }
}
