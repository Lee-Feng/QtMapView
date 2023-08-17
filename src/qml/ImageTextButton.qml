import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.14

Item {
    id: root
    width: Math.max(lable.width,img.width) + 10
    height: header.height - 10

    property alias lableText: lable.text
    /// 松开显示的文件
    property string releasedImage: ""
    /// 按下显示的文件
    property string pressedImage: ""
    /// 字体
    property alias font: lable.font
    /// 图片信息
    property alias icon: img
    /// 是否图片闪烁，开启闪烁之后图标将在iconBlinkStartColor~iconBlinkEndColor之间闪烁
    property bool iconBlink: false
    /// 闪烁间隔时间毫秒
    property int iconBlinkGapMs: 300
    /// 闪烁的起始颜色
    property color iconBlinkStartColor: 'red'
    /// 闪烁的起始颜色
    property color iconBlinkEndColor: 'white'

    /// 点击信号
    signal clicked()

    Label{
        id:lable
        text: cfg.productName
        color: cfg.textColor
        anchors.bottom: root.bottom
        anchors.horizontalCenter: root.horizontalCenter
    }
    Image{
        id:img
        source: releasedImage
        height: root.height - lable.height
        width: height
        fillMode: Image.PreserveAspectFit
        anchors.top: root.top
        anchors.horizontalCenter: root.horizontalCenter
    }
    ColorOverlay {
        id:binkCover
        visible: iconBlink
        anchors.fill:       img
        source:             img
        color:              cfg.textColor
    }
    Timer{
        id:binkTimer
        interval: iconBlinkGapMs
        repeat: true
        running: iconBlink
        onTriggered: {
            if(binkCover.color == iconBlinkStartColor){
                binkCover.color = iconBlinkEndColor
                lable.color = iconBlinkEndColor
            } else {
                binkCover.color = iconBlinkStartColor
                lable.color = iconBlinkStartColor
            }
        }
        onRunningChanged: {
            if(!running){
                lable.color = cfg.textColor
                binkCover.color = cfg.textColor
            }
        }
    }


    MouseArea{
        anchors.fill: parent
        onPressed:{
            if(root.pressedImage != ""){
                img.source = root.pressedImage
            }
        }
        onReleased: {
            if(root.releasedImage != ""){
                img.source = root.releasedImage
            }
        }
        onCanceled: {
            var empty = {}
            released(empty)
        }
        onClicked: {
            root.clicked();
        }
    }
}

