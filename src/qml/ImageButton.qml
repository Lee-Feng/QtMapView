import QtQuick 2.9
import QtQuick.Controls 2.3

Image {
    id: root
    source:"qrc:/logo.png"
    fillMode: Image.PreserveAspectFit

    /// 松开显示的文件
    property string releasedImage: ""
    /// 按下显示的文件
    property string pressedImage: ""
    /// 日志用
    property string name: ""
    /// 是否开启自动重复
    property bool autoRepeat: false
    /// 是否触发了长按
    property bool _longPressTriggered: false
    /// 是否触发了自动重复
    property bool _autoRepeated: false

    signal pressed()
    signal released()
    signal clicked(bool isLongPressTriggered)
    signal longPressed(bool released)

    onReleasedImageChanged: {
        source = root.releasedImage
    }

    MouseArea{
        anchors.fill: parent
        onPressed:{
            _longPressTriggered = false
            _autoRepeated = false
            /// 计时，如果时间按够了就触发长按
            timer.start()
            if(root.pressedImage != ""){
                source = root.pressedImage
            }

            if(autoRepeat){
                autoRepeatTimmer.restart()
            }
            root.pressed()
        }
        onReleased: {
            timer.stop()
            autoRepeatTimmer.stop()
            if(_longPressTriggered){
                console.log(name + "长按松开")
                longPressed(true);
            }

            if(root.releasedImage != ""){
                source = root.releasedImage
            }

            root.released()
        }
        onCanceled: {
            var empty = {}
            released(empty)
        }

        onClicked: {
            if(_autoRepeated){
               return
            }
            root.clicked(_longPressTriggered);
        }
    }

    Timer{
        id:timer
        repeat: false
        interval: 500
        onTriggered: {
            /// 时间到，触发长按
            console.log(name + "长按按下..")
            _longPressTriggered = true
            longPressed(false)
        }
    }

    Timer{
        id:autoRepeatTimmer
        interval: 100
        repeat: true
        onTriggered: {
            if(autoRepeat){
                // 自动重复
                root.clicked(false);
            }
        }
    }
}

