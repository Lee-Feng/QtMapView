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
import QtQuick.Controls 2.2

Rectangle{
    id : root

    ///FUCK!!这里height不能用parent
//    height: parent.height

    height: textFiled.height;
    width: parent.width

    property alias textFiled:textFiled
    property alias validator:textFiled.validator
    property alias nameWidth:lable.width
    property alias valueWidth:textFiled.width
    property alias name:lable.text
    property alias placeholderText: textFiled.placeholderText
    property alias text: textFiled.text
    property alias textItem: textFiled
    property alias readOnly: textFiled.readOnly
    property alias textEditEnable: textFiled.enabled
    property alias inputMethodHints: textFiled.inputMethodHints

    signal valueChanged(string text);

    Label {
        id:lable
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
    }
    TextField {
        id:textFiled
        focus: true
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: lable.right
        anchors.leftMargin: 5
        width:root.width-lable.width
        onTextChanged: valueChanged(text)
        placeholderText: "请输入" + lable.text;
        verticalAlignment: TextInput.AlignBottom
    }
}


