import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3

Popup {
    id: root
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width, parent.height) / 3 * 2
    height: content.implicitHeight + topPadding + bottomPadding
    modal: true
    focus: true

    contentItem: ColumnLayout {
        id: content
        spacing: 20

        Label {
            text: "提示"
            font.bold: true
            font.pixelSize: 18
        }

        Label {
            text: "是否退出当前应用?"
            wrapMode: Label.Wrap
            font.pixelSize: 14
        }

        CheckBox{
            id: closeCamera
            checked: settings.powerOffCameraWhenExit
            text: "退出时也关闭相机"
            onClicked: {
                settings.powerOffCameraWhenExit = checked
            }
        }

        RowLayout {
            spacing: 10

            Button {
                id: cancleButton
                text: "暂不"
                onClicked: root.close()

                Material.foreground: cfg.mainColor
                Material.background: "transparent"
                Material.elevation: 0

                Layout.preferredWidth: 0
                Layout.fillWidth: true
            }

            Button {
                id: exitAndPowerOffButton
                text: "退出"
                onClicked: {
                    if(closeCamera.checked){
                        if(!cameraController.powerOffCamera()){
                            return;
                        }
                    }
                    Qt.quit()
                }
                Material.foreground: cfg.mainColor
                Material.background: "transparent";
                Material.elevation: 0

                Layout.preferredWidth: 0
                Layout.fillWidth: true
            }


        }
    }
}
