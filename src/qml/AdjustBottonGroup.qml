import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

Item {
    id: root
    width:bottonSize * 3 + layout.spacing * 2
    height: bottonSize

    property color background: "#235794"
    property real backgroundOpacity: 0.5
    property string text: "功能"
    property color textColor: "white"
    property int bottonSize: 40

    signal addPressed();
    signal addReleased();
    signal addClicked();

    signal subPressed();
    signal subReleased();
    signal subClicked();

    RowLayout{
        id: layout
        width: parent.width
        height: parent.height
        Item{
            width: root.bottonSize
            height: root.bottonSize
            Rectangle{
                id:lableArea
                color: root.background
                anchors.fill: parent
                radius: 3
                opacity: root.backgroundOpacity
            }
            Label{
                id:lable
                text: root.text
                color: root.textColor
                horizontalAlignment:Text.AlignHCenter
                anchors.centerIn: parent
            }
            /// 避免点击穿透
            MouseArea{
                anchors.fill: parent
            }
        }

        Item{
            width: root.bottonSize
            height: root.bottonSize
            Rectangle{
                id:decArea
                color: root.background
                opacity: root.backgroundOpacity
                anchors.fill: parent
                radius: decArea.width/2
            }
            ImageButton{
                source: "qrc:/add.png"
                anchors.fill: parent
                autoRepeat: true

                onPressed: {
                    addPressed()
                }
                onReleased: {
                    addReleased()
                }

                onClicked: {
                    addClicked();
                }
            }
        }


        Item{
            width: root.bottonSize
            height: root.bottonSize
            Rectangle{
                id:incArea
                color: root.background
                opacity: root.backgroundOpacity
                anchors.fill: parent
                radius: incArea.width/2
            }
            ImageButton{
                source: "qrc:/sub.png"
                anchors.fill: parent
                autoRepeat: true

                onPressed: {
                    subPressed()
                }
                onReleased: {
                    subReleased()
                }

                onClicked: {
                    subClicked();
                }
            }
        }
    }
}





