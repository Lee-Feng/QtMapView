import QtQuick 2.9
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

import "../../"
Item {
    id: r
    property int percent: 0
    property int lineWidth: 10

    enum BarType {
        Text,
        SucceedOrFailed,
        NoBar
    }
    property color __backColor: "#DCDCDC"
    property color __blueColor: "#FF9800"
    property color __succeedColor: "#52c41a"
    property color __failedColor: "#f5222d"

    property color backgroundColor: __backColor
    property color frontColor: {
        switch (barType) {
        case TCircleProgress.BarType.SucceedOrFailed:
            return percent === 100 ? __succeedColor : __failedColor
        default:
            return __blueColor
        }
    }

    property string text: String("%1%").arg(percent)
    property var barType: TCircleProgress.BarType.Text
    Rectangle {
        id: back
        color: "transparent"
        anchors.fill: parent
        border.color: percent === 100 ? frontColor : backgroundColor
        border.width: lineWidth
        radius: width / 2
    }
    Text {
        id: t
        enabled: barType === TCircleProgress.BarType.Text
        visible: enabled
        text: r.text
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    Image {
        id: image
        source: percent === 100 ? "ok.png" : "fail.png"
        enabled: barType === TCircleProgress.BarType.SucceedOrFailed
        visible: enabled
        scale: 2
        anchors.centerIn: parent
    }
    ConicalGradient {
        anchors.fill: back
        source: back
        enabled: percent != 100
        visible: enabled
        smooth: true
        antialiasing: true
        gradient: Gradient {
            GradientStop { position: 0.0; color: frontColor }
            GradientStop { position: percent / 100 ; color: frontColor }
            GradientStop { position: percent / 100 + 0.001; color: backgroundColor }
            GradientStop { position: 1.0; color: backgroundColor }
        }
    }
}
