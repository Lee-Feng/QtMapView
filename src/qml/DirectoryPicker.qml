/*
    Copyright (C) 2017 Aseman Team
    http://aseman.co

    AsemanQtTools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AsemanQtTools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Models 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import Qt.labs.folderlistmodel 2.2

Rectangle {
    id: root
    clip: true

    property string folder: AsemanApp.startPath
    property alias title: header.text
    property alias showDirs: fileMode.showDirs
    property alias showFiles: fileMode.showFiles
    property string folderIcon: "qrc:/ic_folder_open_black_48dp.png"
    property string fileIcon: "qrc:/ic_insert_drive_file_black_48dp.png"
    property real itemMargin: 5
    /// 选择按钮的文字
    property alias selectText: selectBtn.text
    property bool showHeader: true

    signal selectedPath(string dir)

    FileSystemModel {
        id:fileMode
        folder: root.folder
        showDirs: true
        showFiles : false
    }


    Text {
        id: curFilePath
        text: "当前目录:"+fileMode.folder.replace("file:///", "►").replace(new RegExp("//",'g'), "►").replace(new RegExp("/",'g'), "►")
        renderType: Text.NativeRendering
        elide: Text.ElideMiddle
        verticalAlignment: Text.AlignBottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: showHeader ? header.bottom : parent.top
        height: 30*Devices.density
        anchors.leftMargin: itemMargin
        anchors.rightMargin: itemMargin
    }

    ClickableLable{
        id:toParentDir
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: curFilePath.bottom
        horizontalAlignment:Text.AlignLeft
        width: parent.width
        height: 40*Devices.density
        text: "<上级目录"
        color:"black"
        onClicked: listv.model.folder = listv.model.parentFolder
    }

    AsemanListView {
        id: listv
        width: parent.width
        anchors.top: toParentDir.bottom
        anchors.bottom: selectBtn.top
        clip: true
        model: fileMode

        delegate: ItemDelegate {
            id: item
            width: parent.width
            height: 40*Devices.density
            text: model.fileName
            leftPadding: image.width + itemMargin*2
            rightPadding: itemMargin
            Image {
                id: image
                height: 24*Devices.density
                width: height
                anchors.left: parent.left
                anchors.leftMargin: itemMargin
                anchors.verticalCenter: parent.verticalCenter
                source: model.fileIsDir ? folderIcon : fileIcon
            }
            onClicked: {
                if(model.fileIsDir)
                    listv.model.folder = model.filePath
                else
                    Qt.openUrlExternally(model.fileUrl)
            }
        }
    }

    Button {
        id: selectBtn
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10*Devices.density
        text: qsTr("确认选择")
        onClicked: selectedPath(fileMode.folder)
    }


    HScrollBar {
        anchors.right: listv.right
        anchors.top: listv.top
        height: listv.height
        width: 6*Devices.density
        color: cfg.mainColor
        scrollArea: listv
    }

    Header {
        id: header
        visible: showHeader
        width: parent.width
        color: cfg.mainColor
        text: qsTr("选择目录")
        shadow: true
    }

    ClickableLable{
        id:closeBtn
        visible: showHeader
        width: Devices.standardTitleBarHeight
        height: Devices.standardTitleBarHeight
        y: View.statusBarHeight
        anchors.right: parent.right

        text: "关闭"
        onClicked: BackHandler.back()
    }

}
