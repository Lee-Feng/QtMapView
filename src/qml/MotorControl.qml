import QtQuick 2.9
import QtQuick.Controls 2.3
import LaserClearingDevInterface 1.0

MouseArea{
    id:mouse
    anchors.fill: parent
    property var cameraView

    property bool __goto_forward: false
    property var __axis: API.Axis_all

    /// 避免上层手势捕获
    preventStealing : true

    onPressed: {
        /// 根据点击位置，决策是左右还是上下移动
        let x_shift =  mouse.x - width/2
        let y_shift =  mouse.y - height/2

        if(Math.abs(x_shift) > Math.abs(y_shift)){
            __axis = API.Axis_1
            __goto_forward = x_shift > 0
            console.log("云台横向移动",__goto_forward?"→":"←")
        } else {
            __axis = API.Axis_2
            __goto_forward = y_shift > 0
            console.log("云台纵移动",__goto_forward?"↓":"↑")
        }
        $api.startHandPtzMove(__axis,__goto_forward)
    }
    onReleased: {
        stop();
    }
    onCanceled: {
        stop();
    }

    function stop(){
        $api.stopHandPtzMove(__axis)
    }

    BlinkLable{
        id:alert
        width: cameraView.width
        visible: mouse.enabled && infoTimeout.running
        topPadding: 20
        color: "green"
        horizontalAlignment:Text.AlignHCenter
        text: "点击图像窗口可以调整瞄准方向"

        Timer{
            id:infoTimeout
            interval: 15000
            repeat: false
            running: mouse.enabled
            onTriggered: {
                console.log("隐藏'" + alert.text + "'提示")
            }
        }
        onVisibleChanged: {
            if(visible){
                /// 显示提示信息，只显示几秒钟即可
                infoTimeout.restart()
            }
        }
    }

}
