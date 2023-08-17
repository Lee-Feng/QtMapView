import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
//import QtSystemInfo 5.0
// Aseman扩展的一定要放到Qt自带的后面，相同名字的组件就会被覆盖掉
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0

import AsemanQml.MaterialIcons 2.0
import AsemanQml.Awesome 2.0

Header {
    id: header
    color: cfg.mainColor
    shadow: false

    property alias titleText: textData.text
    property color buttonColor: cfg.textColor
    property real buttonRatio: 0

    property bool useFontAwsome: true
    //
    // 参考QtAseman\src\asemanqml\awesome\Awesome\Awesome.qml
    // 参考http://www.fontawesome.com.cn/faicons/
    //
    // 通过用FontCreator打开fontawesome-webfont.ttf文件(版本4.4.1)
    // 根据Awesome.qml中定义的编码，比如\uf0c8,在FontCreator中查找编码是f0c8的图标就知道长啥样了
    property string optionTxt: useFontAwsome ? Awesome.fa_ellipsis_v : "选择"
    property int optionFontSize: useFontAwsome ? 12 : 10

    /// 文字两边的空白处，为了避免与按钮重叠
    property real sidePadding: 50

    // 是否显示右侧的功能按钮
    property bool showOptionButton: false
    // 是否显示左侧的功能按钮
    property bool showMenuButton: true

    signal menuClicked()
    signal titleClicked()
    signal optionClicked()

    Text {
        id: textData
        font.pixelSize: Math.floor(10*Devices.fontDensity)
        font.family: AsemanApp.globalFont.family

        color: cfg.textColor

        text: ""

        wrapMode: Text.Wrap

        elide: Text.ElideRight
//        lineHeightMode: Text.ProportionalHeight //设置行间
//        /// 0.5倍行距
//        lineHeight: 0.95*Devices.fontDensity

        maximumLineCount:2

        width: parent.width - sidePadding * 2
        height: parent.height

        anchors.verticalCenter: header.verticalCenter
        anchors.verticalCenterOffset: (Devices.transparentStatusBar ? View.statusBarHeight : 0) / 2
        x: {
            return sidePadding
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        MouseArea{
            anchors.fill: parent
            onClicked: {
                titleClicked()
            }
        }
    }

    ClickableLable{
        id:icon
        visible: header.showOptionButton
        fontFamily: useFontAwsome ? Awesome.family : ""
        text:optionTxt
        color: header.buttonColor
        fontSize: optionFontSize
        width: Devices.standardTitleBarHeight
        height: Devices.standardTitleBarHeight
        y: View.statusBarHeight
        anchors.right: parent.right
        onClicked: header.optionClicked();
    }

    HeaderMenuButton {
        id: menuButton
        ratio: header.buttonRatio
        buttonColor: header.buttonColor
        visible: header.showMenuButton
        onClicked:header.menuClicked()
    }
}


