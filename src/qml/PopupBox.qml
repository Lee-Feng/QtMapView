import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Models 2.0
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0

///TODO: 还没实现
Dialog {
    id: root
    modal: true
    focus: true
    // 用于展示内容区间的高度
    contentHeight: Math.min(content.height, parent.height*0.8)
    topPadding: 10
    bottomPadding: 10
    width: Math.min(300, parent.width*0.9)
    anchors.centerIn: parent

    AsemanFlickable {
        id: flick
        clip: true
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 0
        contentWidth: content.width
        contentHeight: content.height

        Item{
            id: content
            width: flick.width
        }
    }

    HScrollBar {
        anchors.right: flick.right
        anchors.top: flick.top
        height: flick.height
        width: 6*Devices.density
        color: "#18f"
        scrollArea: flick
    }
}
