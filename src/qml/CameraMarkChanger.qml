import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
import ImageItem 1.0

Item{
    id: marker
    anchors.fill: parent

    property real moveSpeed: 1
    property real moveAcc: 5
    /// 是否开启调整准星
    property bool marking: false

    /// 相机视图
    property CameraView cameraView

    BlinkLable{
        id:alert
        visible: marker.marking
        width: parent.width
        topPadding: 20
        color: "green"
        horizontalAlignment:Text.AlignHCenter
        text: "点击图像窗口可以移动光标"
    }

    MouseArea{
        id:mouse
        anchors.fill: parent
        enabled: marker.marking
        onPressed: {
            marker.moveSpeed = 1
            click_timer.start()
        }
        onReleased: {
            click_timer.stop()
        }
        onCanceled: {
            click_timer.stop()
        }

        onClicked: {
            marker.moveAcc = 5
            marker.moveSpeed = 1
            marker.moveMarkPosByClickPos();
        }
    }

    Timer{
        id:click_timer
        interval: 200
        repeat: true
        onTriggered: {
            marker.moveMarkPosByClickPos();
        }
    }

    function moveMarkPosByClickPos(){
        var m_x = cameraView.markPosX
        var m_y = cameraView.markPosY
        var click_x = cameraView.convertToImagePosX(mouse.mouseX)
        var click_y = cameraView.convertToImagePosY(mouse.mouseY)


        var new_pos = 0
        /// 为了控制稳定，一次只移动一个方向
        if(Math.abs(m_x-click_x) > Math.abs(m_y-click_y)){
            if(marker.moveSpeed > Math.abs(m_x-click_x)){
                marker.moveSpeed = Math.abs(m_x-click_x);
            }
            new_pos = m_x + (click_x > m_x ? marker.moveSpeed : -(marker.moveSpeed));
            console.log("请求x移动到：" + m_x + "-->" + new_pos + "(" + click_x + ")")
            cameraView.markPosX = new_pos
        } else {
            if(marker.moveSpeed > Math.abs(m_y-click_y)){
                marker.moveSpeed = Math.abs(m_y-click_y);
            }
            new_pos = m_y + (click_y > m_y ? marker.moveSpeed : -(marker.moveSpeed));
            console.log("请求y移动到：" + m_y + "-->" + new_pos + "(" + click_y + ")")
            cameraView.markPosY = new_pos
        }

        marker.moveSpeed += marker.moveAcc
        if(marker.moveSpeed > 100){
            marker.moveSpeed = 100;
        }
    }
}
