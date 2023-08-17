import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id:root
    property alias orientation: slider.orientation
    property alias from: slider.from
    property alias to: slider.to
    property alias value: slider.value
    property bool opened: false
    property alias sliderX : slider.x
    property alias sliderY : slider.y
    /// 每全局移动多少像素就进行一次增加和减少
    property int sensitivity: 10

    /// 值变稳定了
    signal valueChangedStabled()

    function open(){
        opened = true
    }

    function close(){
        opened = false
    }

    function setValue(v){
        slider.value = v
    }

    MouseArea{
        anchors.fill: parent
        enabled: root.opened
        property point pressPoint : Qt.point(0,0)

        onClicked: {
            root.opened = false
        }
        onPressed:{
            pressPoint = Qt.point(mouse.x,mouse.y)
        }
        onPositionChanged:{
            let p = Qt.point(mouse.x,mouse.y)
            let shift = 0
            if(slider.orientation == Qt.Vertical) {
                shift = (pressPoint.y - p.y)
            } else {
                 shift = (p.x - pressPoint.x)
            }
            if(Math.abs(shift) < sensitivity){
                return
            }
            if(shift > 0){
                slider.increase()
            } else {
                slider.decrease()
            }
            pressPoint = p
        }
    }

    Rectangle{
        width: slider.width + 10
        height: slider.height + 10
        x : slider.x - 5
        y : slider.y - 5
        color: "gray"
        opacity: 0.5
        visible: slider.visible
    }

    Slider {
        id:slider
        enabled: root.opened
        visible: root.opened
        value: 0.5

        onValueChanged: {
            delayCall.restart()
        }
    }

    Timer{
        id:delayCall
        interval : 100
        repeat : false
        onTriggered: {
            valueChangedStabled()
        }
    }
}
