import AsemanQml.Base 2.0
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.12

Button {
    id: root
    highlighted: true
    text: "Button"
    /// 文字颜色
    property color color: cfg.textColor
    /// 背景颜色
    /// 可以通过qtquickcontrols2.conf修改Material.accent具体的值
    property color backgroundColor: enabled ? Material.accent : "#CCCCCC"
    property color backgroundPressedColor: Qt.darker(backgroundColor, 1.2)
    property int horizontalAlignment: Text.AlignHCenter
    property int verticalAlignment: Text.AlignVCenter
    //property string fontFamily: ""
    property int fontSize: 10
    /// 冰冻等待
    property bool freeze : false

    /// 超时函数响应了
    signal timeout();

    contentItem: Text {
        text: root.text
        color: root.color
        // 不设置，用app默认字体，避免设置为空就用了系统默认了
        //font.family: fontFamily
        font.pixelSize: Math.floor(fontSize*Devices.fontDensity)
        horizontalAlignment: root.horizontalAlignment
        verticalAlignment: root.verticalAlignment
        width: root.width
        height: root.height
    }

    background: Rectangle {
        implicitWidth: 83
        implicitHeight: 37
        color: backgroundColor
        radius: 3
        layer.enabled: !flat
        layer.effect: DropShadow {
            transparentBorder: true
            color: root.down ? root.backgroundPressedColor : root.backgroundColor
            samples: 20
        }
    }

    function tryUnfreezeAfterTimeout(timeout){
        if(!timeout){
            timeout = 3000
        }
        console.log("freeze ....")
        root.freeze = true
        autoUnlockFrezze.interval = timeout
        autoUnlockFrezze.restart()
    }

    Timer{
        id:autoUnlockFrezze
        repeat: false
        onTriggered: {
            if(root.freeze){
                root.freeze = false
                timeout();
                toast.makeToast("请求超时")
            }
        }
    }

    onClicked: {
        if(settings.btnVibrateOn){
            $system.vibrate(80)
        }
    }
}
