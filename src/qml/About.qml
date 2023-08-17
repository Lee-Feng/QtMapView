import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0

Dialog {
    id: aboutDialog
    modal: true
    focus: true
    title: "关于"
    contentHeight: aboutColumn.height
    contentWidth: aboutColumn.width

    Column {
        id: aboutColumn
        spacing: 20

        Label {
            width: aboutDialog.availableWidth - aboutColumn.spacing - 10
            text: "这是" + cfg.productName +"。"
            wrapMode: Label.Wrap
            font.pixelSize: 12
        }

        Label {
            width: aboutDialog.availableWidth - aboutColumn.spacing - 10
            wrapMode: Label.Wrap
            text: cfg.discribe + "\n" +
                  "\n\n版本信息:" + cfg.versionString
            font.pixelSize: 12
        }
    }
}
