import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Models 2.0
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import "qrc:/TaoQuick/BasicComponent/Progress/" as TaoQuick

Dialog {
    id: root
    modal: true
    focus: true
    // 用于展示内容区间的高度
    contentHeight: Math.min(column.height, parent.height*0.8)
    topPadding: 10
    bottomPadding: 10
    width: Math.min(300, parent.width*0.9)
    anchors.centerIn: parent
    closePolicy : Popup.NoAutoClose
    property bool blockBack: true

    property alias percent: bar.percent
    property string message: msg.text


    signal requestCancel()

    AsemanFlickable {
        id: flick
        clip: true
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 0
        contentWidth: column.width
        contentHeight: column.height


        Row {
            id: column
            width: flick.width
            spacing: 10

            TaoQuick.TCircleProgress{
                id:bar
                width: flick.width * 0.3
                height: flick.width * 0.3
                percent : 0
                //NumberAnimation on percent { from: 0; to: 100; duration: 5000; running: true; loops: Animation.Infinite}
            }

            Rectangle{
                width: column.width - bar.width - column.spacing
                height: parent.height
                Text {
                    id: msg
                    text: message
                    anchors.top: parent.top
                    anchors.bottom: cancel.top
                    anchors.bottomMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: parent.height - cancel.height - anchors.bottomMargin
                    wrapMode:Text.WrapAnywhere
                    maximumLineCount: 2
                    elide: Text.ElideMiddle
                }

                ClickableLable{
                    id:cancel
                    text: "取消"
                    anchors.bottom: parent.bottom
                    height: 30*Devices.density
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: cfg.mainColor
                    onClicked: {
                        requestCancel()
                    }
                }
            }
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

    onVisibleChanged: {
        if(visible){
            /// 这里需要AsemanWindow设置backController: true
            BackHandler.pushHandler(this, function(){
                if(visible && blockBack)
                    return false
                else
                    BackHandler.back()
            })
        } else {
            BackHandler.removeHandler(this)
        }
    }
}
