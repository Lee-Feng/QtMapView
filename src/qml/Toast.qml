import QtQuick 2.9
import QtQuick.Controls 2.3
/// 通知栏
Rectangle{
    id : root
    ///property real pos_x: value
    visible: false
    width: 0
    height: 0

    Rectangle{
        id: toast_background
        color: "#C0C0C0"
        opacity:0.9
        layer.enabled : true
        width: txt.width + 20
        height: txt.height + 10
        anchors.verticalCenter:root.verticalCenter
        anchors.horizontalCenter:root.horizontalCenter
    }

    Label{
        id: txt
        color: 'black'
        anchors.centerIn: toast_background
    }

    Timer{
        id:timer
        interval: 3000
        repeat: false
        onTriggered: {
            visible = false
        }
    }

    function makeToast(msg){
        makeToastForTime(msg,3000)
    }
    function makeToastForTime(msg,time){
        txt.text = msg
        root.anchors.horizontalCenter = parent.horizontalCenter
        root.anchors.top = parent.top
        root.anchors.topMargin = 100
        root.visible = true;
        console.log("Toast:" + msg)
        timer.interval = time ? time : 3000
        timer.restart()
    }
}
