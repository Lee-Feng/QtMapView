import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Models 2.0
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0

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
    // 是否点击之后自动退出
    property bool closeOnOption: true

    property string text: ""
    property string button_yes: qsTr("确定")
    property string button_no: qsTr("取消")

    signal requestYes()
    signal requestNo()

    /// 回调函数
    property var yesCallback;
    property var noCallback;
    property var closeCallback;

    onClosed: {
        if(closeCallback){
            closeCallback()
        }
    }

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

        Column {
            id: column
            width: flick.width
            spacing: 10
            Text {
                id:textContent
                width: parent.width
                text: root.text
                wrapMode: Text.WordWrap
            }
            RowLayout {
                spacing: 10
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                MyButton {
                    id:btnNo
                    text: button_no
                    visible: button_no !== ""
                    onClicked: {
                        if(noCallback){
                            noCallback();
                        }
                        requestNo();
                        if(closeOnOption){
                            close()
                        }
                    }
                }
                MyButton {
                    id:btnYes
                    text: button_yes
                    visible: button_yes !== ""
                    onClicked: {
                        if(yesCallback){
                            yesCallback();
                        }
                        requestYes();
                        if(closeOnOption){
                            close()
                        }
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
}
