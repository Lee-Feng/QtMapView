import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
//import QtSystemInfo 5.0
// Aseman扩展的一定要放到Qt自带的后面，相同名字的组件就会被覆盖掉
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0
import AsemanQml.MaterialIcons 2.0
import AsemanQml.Awesome 2.0
import MJPEGCapture 1.0
import ImageItem 1.0
import QtQuick.Layouts 1.3
import QtMultimedia 5.4

Rectangle {
    /// 通常用root作为根的id，子对象才方便访问属性
    id:root

    width: parent.width
    height: parent.height

    property bool _cameraReady: false

    MyHeader{
        id: header
        width: parent.width
        showOptionButton: false
        showMenuButton: false
        useFontAwsome:false
        buttonRatio: drawer.percent
        buttonColor: buttonRatio > 0.1? "black" : "white"
        onMenuClicked: drawer.percent? drawer.discard() : drawer.show()
//        LocationLable{}
//        onTitleClicked:settingDialog.open()
    }

    ImageTextButton{
        id:logo
        height: header.height - 10
        releasedImage:"qrc:/logo.png"
        pressedImage:"qrc:/logo.png"
        lableText:cfg.productName
        anchors.left: header.left
        anchors.verticalCenter: header.verticalCenter
        onClicked: {
            toast.makeToast(cfg.company + lableText)
        }
    }



    Drawer {
        id: drawer
        /// 放到右边
        LayoutMirroring.enabled: true
        LayoutMirroring.childrenInherit: true
        anchors.fill: parent
        delegate: Rectangle {
            anchors.fill: parent
            AppMenu {
                id:menu
            }
        }
    }

    /// 与Header分开，目的是放在Drawer上面
    HeaderMenuButton {
        id: menuButton
        /// 放到右边
        LayoutMirroring.enabled: true
        ratio: header.buttonRatio
        buttonColor: header.buttonColor
        onClicked:header.menuClicked()
    }

    Component.onCompleted: {
        console.log("完成...")
    }
}
