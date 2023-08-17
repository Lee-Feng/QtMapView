import QtQuick 2.9
import QtQuick.Controls 2.3

Label{
    id:alert
    topPadding: 20
    color: "red"
    horizontalAlignment:Text.AlignHCenter
    text: "点击图像窗口可以移动光标"

    onVisibleChanged: {
        if(visible){
            alert.opacity = 1
        }
    }

    onTextChanged: {
        alert.opacity = 1.0
        sequential.restart()
    }

    SequentialAnimation{
        id: sequential
        running:  alert.visible
        loops:Animation.Infinite
        /// 到不透明
        NumberAnimation{
            target: alert
            properties: "opacity"
            to: 1.0
            duration: 400
            easing.type: Easing.OutBack //改变缓和曲线类型
        }
        /// 到全透明
        NumberAnimation{
            target: alert
            properties: "opacity"
            to: 0.0
            duration: 400
            easing.type: Easing.OutBack
        }
    }
}
