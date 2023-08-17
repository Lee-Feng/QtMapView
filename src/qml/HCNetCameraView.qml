import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls 2.3 as QC
import Qt.labs.folderlistmodel 2.2
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0
import MJPEGCapture 1.0
import QmlProxyWidget 1.0
import PlusItem 1.0
import QtQuick.Controls.Material 2.12
import AsemanQml.MaterialIcons 2.0
import AsemanQml.Awesome 2.0

QmlProxyWidget {
    id: cameraView
    clip: true
    widget:$HCNetCameraRenderView

    /// mark的位置
    property int markPosX : $api.markerPosX
    property int markPosY : $api.markerPosY

    // 提示信息是否可见
    property alias lableVisible: mesg.visible

    PlusItem {
        id:plus
        color: "red"
        width: 20
        height: 20
        size: 1
    }

    Label{
        id:mesg
        anchors.horizontalCenter: parent.horizontalCenter
    }

    /// 更新mark的位置
    function updateMarkPos(){
//        plus.x = cameraView.convertToUIPosX(markPosX) - plus.width/2
//        plus.y = cameraView.convertToUIPosY(markPosY) - plus.height/2
    }

    onMarkPosXChanged: {
        updateMarkPos();
    }
    onMarkPosYChanged: {
        updateMarkPos();
    }
    onHeightChanged: {
        updateMarkPos()
    }
    onWidthChanged: {
        updateMarkPos()
    }

    Timer{
        id: autorefresh
        repeat: true
        interval: 20
        running: true
        onTriggered: {
            cameraView.refresh()
        }
    }

    /// 修复初始值不正确的bug
    Timer{
        id: fixNoUpdateBugTimer
        repeat: false
        interval: 100
        onTriggered: {
            cameraView.updateMarkPos()
        }
    }
    Component.onCompleted: {
        fixNoUpdateBugTimer.start()
        $api.startCamera()
    }
    Component.onDestruction: {
        $api.stopCamera()
    }
}
