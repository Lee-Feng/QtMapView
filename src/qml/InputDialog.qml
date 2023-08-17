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
    title: "请输入"
    // 用于展示内容区间的高度
    contentHeight: Math.min(250,parent.height*0.8)
    topPadding: 10
    bottomPadding: 10

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width, parent.height) / 3 * 2
    height: Math.max(Math.min(parent.width, parent.height) / 5 * 2,150)

    property real lableWidth: 0.2*width
    property real inputWidth: width - lableWidth
    /// 输入项目名字
    property alias feildName: item.name
    property alias feildValue: item.text
    /// 取消自动关闭
    property bool autoCloseOnCancel: false
    /// 保存自动关闭
    property bool autoCloseOnSave: false
    /// 用户数据
    property var userData;
    /// 保存的回调函数
    property var saveCallback;
    /// 输入验证器
    property alias validator: item.validator
    /// 输入类型
    property alias inputMethodHints: item.inputMethodHints

    signal requestSave()
    signal requestCancel()

    /// 按钮使能
    property bool saveEnable : true
    property bool cancelEnable : true


    function clear(){
        feildValue = "";
        sequential.stop()
        error_text.opacity = 0
    }

    // 显示错误信息
    function showErrorString(str){
        error_text.text = str
        error_text.opacity = 1
        sequential.restart()
    }

    Connections{
        target:$api
        enabled: root.visible
        onError:{
            console.log("输入对话框拦截api错误并显示:" + error)
            showErrorString(error)
        }
    }

    SequentialAnimation{
        id: sequential
        loops:1
        /// 到不透明
        NumberAnimation{
            target: error_text
            properties: "opacity"
            to: 1.0
            duration: 100
            easing.type: Easing.OutBack //改变缓和曲线类型
        }
        PauseAnimation {  duration: 1000 }
        /// 到全透明
        NumberAnimation{
            target: error_text
            properties: "opacity"
            to: 0
            duration: 100
            easing.type: Easing.OutBack
        }
    }

    Label{
        id:error_text
        width: parent.width - 20
        z:1
        x:0
        y:-10
        elide: Text.ElideLeft
        color: "red"
        horizontalAlignment:Text.AlignHCenter
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

            FormItem{
                id: item
            }

            RowLayout {
                spacing: 10
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter

                MyButton {
                    id:btnNo
                    text: qsTr("取消/关闭")
                    enabled: cancelEnable
                    onClicked: {
                        requestCancel();
                        if(autoCloseOnCancel){
                            close()
                        }
                    }
                }

                MyButton {
                    id:btnYes
                    text: qsTr("确定保存")
                    enabled: saveEnable
                    onClicked: {
                        requestSave();
                        if(saveCallback){
                            saveCallback();
                        }
                        if(autoCloseOnSave){
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
        color: cfg.mainColor
        scrollArea: flick
    }

//    Connections{
//        target: keyhandler
//        enabled: root.visible && root.activeFocus
//        onRemoteKeyTriggered:{
//            if(key == Qt.Key_F11 && press && btnNo.visible && btnNo.enabled){
//                console.log("[遥控] 点击" + btnNo.text)
//                btnNo.clicked()
//            } else if(key == Qt.Key_F12 && press && btnYes.visible && btnYes.enabled){
//                console.log("[遥控] 点击" + btnYes.text)
//                btnYes.clicked()
//            }
//        }
//    }
}
