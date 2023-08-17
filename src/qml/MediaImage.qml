import QtQuick 2.9
import QtQuick.Controls 2.3

import AsemanQml.Base 2.0
import AsemanQml.Awesome 2.0

Rectangle {
    id: root
    color:"black"

    /// baseImage点击了
    signal clicked(string source)
    /// 播放按钮点击了
    signal iconClicked(string source)
    /// 选择框点击了
    signal selectorClicked(string source,bool checked)
    /// 图片分辨率太小了的通知，可以让外界加载大图
    signal baseImageTooSmall();

    /// 底图片
    property string baseImage: ""
    ///
    property alias fillMode: base.fillMode
    /// 描述类型的图片
    property string typeImage: ""
    property int typeWidth: 50
    property int typeHeight: 50
    // 是否可以双指缩放
    property bool enablePinchScale: false

    property alias asynchronous : base.asynchronous;
    property bool cache: false
    property real sourceWidth: 250
    property real sourceHeight: 250

    property real initWidth : Math.min(width,height)
    property real initHeight: Math.max(width,height)
    property int initw: width
    property int inith: height

    property var userdata: ""
    // 是否显示选择图标
    property bool showSelectorIcon: false
    // 当前是否选择了
    property bool selected : false

    /// 不显示时也改为不选择
    onShowSelectorIconChanged: {
        if(!showSelectorIcon){
            selected = false
        }
    }

    Flickable {
        id: flick
        anchors.fill: parent
        anchors.centerIn: parent
        contentWidth: initw
        contentHeight: inith
        z:1
        enabled: true

        PinchArea {
            width: Math.max(flick.contentWidth, flick.width)
            height: Math.max(flick.contentHeight, flick.height)
            enabled:enablePinchScale
            property real initialWidth
            property real initialHeight

            onPinchStarted: {
                console.log("图像pich...")
                initialWidth = flick.contentWidth
                initialHeight = flick.contentHeight
            }

            onPinchUpdated: {
                // adjust content pos due to drag
                flick.contentX += pinch.previousCenter.x - pinch.center.x
                flick.contentY += pinch.previousCenter.y - pinch.center.y

                // resize content
                flick.resizeContent((initialWidth * pinch.scale)<root.width?root.width:(initialWidth * pinch.scale),
                                    (initialHeight * pinch.scale)<root.height?root.height:(initialHeight * pinch.scale), pinch.center)
            }

            onPinchFinished: {
                // Move its content within bounds.
                flick.returnToBounds()
                console.log("当前区域大小:" + flick.contentWidth + " x " + flick.contentHeight)
                console.log("当前图片大小:" + base.sourceSize.width + " x " + base.sourceSize.height)
                if(flick.contentWidth > base.sourceSize.width &&
                        flick.contentHeight > base.sourceSize.height){
                    baseImageTooSmall();
                }
            }


            Image{
                id:base
                cache:root.cache
                asynchronous: true
                width: parent.width
                height: parent.height
                fillMode: Image.PreserveAspectCrop
                source: root.baseImage
//                onSourceSizeChanged: {
//                    console.log("当前区域大小:" + flick.contentWidth + "x" + flick.contentHeight)
//                    console.log("当前图片大小:" + base.sourceSize.width + "x" + base.sourceSize.height)
//                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        if(showSelectorIcon){
                            selector.selectTrigger();
                            return;
                        }

                        console.log("图像base clicked...")
                        root.clicked(baseImage);
                    }
                }
            }

            Image{
                id:icon
                asynchronous:true
                cache:root.cache
                anchors.centerIn: parent
                width: root.typeHeight
                height: root.typeHeight
                fillMode: Image.PreserveAspectFit
                source: root.typeImage
                visible: root.typeImage !== ''

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        if(showSelectorIcon){
                            selector.selectTrigger();
                            return;
                        }

                        console.log("图像icon clicked...")
                        root.clicked(baseImage);
                        root.iconClicked(typeImage);
                    }
                }
            }

            Rectangle{
                id: cover
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
                color: cfg.mainColor
                opacity: 0.5
                visible: showSelectorIcon && root.selected
            }


            ClickableLable{
                id:selector
                flat : true
                visible: showSelectorIcon
                fontFamily: Awesome.family
                text: root.selected ? Awesome.fa_check_square : Awesome.fa_square
                color: header.buttonColor
                fontSize: 10
                width: 36
                height: 36
                anchors.right: base.right
                anchors.bottom: base.bottom
                onClicked: {
                    selectTrigger()
                }
                function selectTrigger(){
                    root.selected = !root.selected
                    selectorClicked(baseImage,root.selected)
                }
            }


        }
    }
}

