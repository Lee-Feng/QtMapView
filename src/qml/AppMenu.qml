import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0

Rectangle{
    id : root
    anchors.fill: parent

    property bool force_upgrade: false

    AsemanFlickable {
        id: flick
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.bottom: parent.bottom
        contentWidth: column.width
        contentHeight: column.height

        Column {
            id: column
            width: flick.width

            MainMenuItem {
                text: "设置线路"
                onClicked: {
                    settingDialog.open()
                }
            }

            MainMenuItem {
                text: "查找设备"
                onClicked: {
                    cameraController.discover()
                }
            }

            MainMenuItem {
                text: "关于"
                onClicked: {
                    aboutDialog.open()
                }
            }
        }
    }

    Column{
        id:locationInfoLable
        visible: cfg.showLocationDebug
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        padding: 10
        spacing : 10
        width: parent.width
        Label{
            id:locationGPSInfo
            horizontalAlignment: Text.AlignLeft
            color: "black"
            wrapMode: Text.Wrap
            maximumLineCount: 3
            width: flick.width - locationInfoLable.padding * 2
            text: "位置信息:--"
        }
    }

    /// 处理相机控制器信号
    Connections {
       target: $locationSys
       onGpsUpdated: {
           var text;
           if(isvalid){
               text = "纬:" + latitude + ", 经:" + longtitude + ", 高:" + altitude
           } else {
               text = "GPS:不可用"
           }
           locationGPSInfo.text =text
       }
    }

    HScrollBar {
        anchors.right: flick.right
        anchors.top: flick.top
        height: flick.height
        width: 6*Devices.density
        color: cfg.mainColor
        forceVisible:true
        scrollArea: flick
    }
}

