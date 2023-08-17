import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls 2.3 as QC
import Qt.labs.folderlistmodel 2.2
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0
import MJPEGCapture 1.0
import ImageItem 1.0
import PlusItem 1.0
import QtQuick.Controls.Material 2.12
import AsemanQml.MaterialIcons 2.0
import AsemanQml.Awesome 2.0

ImageItem {
    id: cameraView
    // 指定了大小之后fillMode才有效果
    fillMode: cfg.cameraViewFillMode
    clip: true

    /// mark的位置,(0,0)是原始图片的左上角
    property int markPosX : $api.markerPosX
    property int markPosY : $api.markerPosY

    /// mark的位置在窗口中显示的位置，不要修改该值
    readonly property int markPosXInView : cameraView.convertToUIPosX(markPosX)
    readonly property int markPosYInView : cameraView.convertToUIPosY(markPosY)

    // 提示信息是否可见
    property alias lableVisible: mesg.visible

    image: $api.currentCameraFrame()

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
        plus.x = cameraView.convertToUIPosX(markPosX) - plus.width/2
        plus.y = cameraView.convertToUIPosY(markPosY) - plus.height/2
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
    onImageSizeChanged: {
        updateMarkPos()
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

    /// 图片刷新
    Connections{
        target:$api
        onCameraImageChanged: {
            cameraView.image = image
        }
    }

//    /// 识别的mark框选更新
//    Connections{
//        target:$api
//        onAutoMeasurerNofity:{
//            mesg.text = data.info;
//            mesg.color = data.is_error ? "red" : "blue";
//            cameraView.setMarks(data.lines);
//        }
//    }
}
