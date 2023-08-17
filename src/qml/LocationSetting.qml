import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Models 2.0
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0

Dialog {
    id: root
    modal: true
    focus: true
    title: "路线信息设置"
    // 用于展示内容区间的高度
    contentHeight: Math.min(365,parent.height*0.61)
    topPadding: 10
    bottomPadding: 10
    clip: true
    property real lableWidth: 0.2*width
    property real inputWidth: width - lableWidth

    /// 线路参数
    property alias line_name: lineName.text
    property string line_dir : ""
    property alias station: lineStation.text
    property alias tunnel: lineTunnel.text
    property alias backboneNum: linebackBoneNum.text
    property alias backboneGap: backbone_gap.value
    property alias forwordIsAcc: backbone_dir.checked
    property alias mileage_revert_enabled: mileage_revert.checked
    property alias mileage_revert_enabled_avaliale: mileage_revert.enabled
    property alias default_max_dropper_count: default_dropper_count.value
    property bool miligateVisible: true

    signal requestSave()
    signal requestCancel()

    onRejected: {
        requestCancel()
    }

    function clear(){
        line_name = ""
        line_dir = ""
        station = ""
        tunnel = ""
        backboneNum  = ""
    }

    AsemanFlickable {
        id: flick
        clip: true
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: buttons.top
        anchors.topMargin: 0
        contentWidth: column.width
        contentHeight: column.height

        Column {
            id: column
            width: flick.width
            spacing: -5

            RowLayout{
                width: parent.width - 20
                spacing: 20
                FormItem{
                    id:lineName
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    name:"线路:"
                }
                RowLayout{
                    Layout.fillWidth: true
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Label{
                        text: "方向:"
                    }
                    Switch{
                        id:lineDir
                        checked : line_dir === "上行"
                        text: "上行"
                        onCheckedChanged: {
                            if(checked){
                                line_dir = "上行"
                            } else {
                                line_dir = "下行"
                            }
                        }
                    }
                    Rectangle{
                        Layout.fillWidth: true
                    }
                }
            }


            RowLayout{
                width: parent.width - 20
                spacing: 20
                FormItem{
                    id:lineStation
                    Layout.fillWidth: true
                    name:"站区:"
                }
                FormItem{
                    id:lineTunnel
                    Layout.fillWidth: true
                    name:"隧道:"
                }
            }

            RowLayout{
                width: parent.width - 20
                spacing: 20
                RowLayout{
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    FormItem{
                        Layout.preferredWidth: parent.width * 0.5
                        Layout.fillWidth: true
                        id:linebackBoneNum
                        name:"杆号:"
                    }
                    Switch{
                        id:backbone_dir
                        text: "递增"
                    }
                }

                RowLayout{
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    Label{
                        text: "间隔:"
                    }
                    SpinBox {
                        Layout.maximumWidth: 120
                        id:backbone_gap
                        width: 40
                        height: 30
                        value: 2
                        from: 1
                        to: 10
                        editable: true
                        onValueChanged:{
                        }
                    }
                    Rectangle{
                        Layout.fillWidth: true
                    }
                }
            }


            RowLayout{
                width: parent.width - 20
                spacing: 20
                visible: miligateVisible

                RowLayout{
                    clip: true
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    Label{
                        text: "里程:"
                    }

                    Label{
                        //text:$api.mileageFormat($api.devInfo.mileage)
                        Layout.minimumWidth: 50
                    }
                    MyButton {
                        text: qsTr("清零")
                        Layout.maximumWidth: 50
                        onClicked: {
                            if($api.resetMileage(0)){
                                toast.makeToast("已清零")
                            }
                        }
                    }
                    MyButton {
                        text: qsTr("设置")
                        Layout.maximumWidth: 50
                        onClicked: {
                            inputDoubleDialog.feildName = "里程值(km)"
                            inputDoubleDialog.title = "输入当前里程值:"
                            inputDoubleDialog.feildValue = "" + $api.getMileage() / 1000 / 1000
                            inputDoubleDialog.saveCallback = function(){
                                /// 根据输入值校准
                                let newvalue = parseFloat(inputDoubleDialog.feildValue) * 1000 * 1000
                                console.log("里程重置为:" + newvalue)
                                $api.resetMileage(newvalue)
                            };
                            inputDoubleDialog.open()
                        }
                    }

                    Rectangle{
                        Layout.fillWidth: true
                    }
                }

                RowLayout{
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    Label{
                        text: "反转:"
                    }
                    Switch{
                        id:mileage_revert
                        text: "前进时里程减小"
                        Image {
                            id:waring_icon
                            visible: mileage_revert.checked
                            source: "qrc:/warn.png"
                            width: parent.height * 0.5
                            height: parent.height * 0.5
                            anchors.left: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            property real bg_opacity: 0.5
                            property real during: 200

                            SequentialAnimation{
                                id: sequential
                                running: waring_icon.visible
                                loops:Animation.Infinite
                                /// 到不透明
                                NumberAnimation{
                                    target: waring_icon
                                    properties: "opacity"
                                    to: 1.0
                                    duration: waring_icon.during
                                    easing.type: Easing.OutBack //改变缓和曲线类型
                                }
                                /// 到全透明
                                NumberAnimation{
                                    target: waring_icon
                                    properties: "opacity"
                                    to: waring_icon.bg_opacity
                                    duration: waring_icon.during
                                    easing.type: Easing.OutBack
                                }
                            }
                        }
                    }
                    Rectangle{
                        Layout.fillWidth: true
                    }
                }
            }

            RowLayout{
                width: parent.width - 20
                spacing: 20

                RowLayout{
                    Layout.preferredWidth: parent.width * 0.5 - 10
                    Layout.fillWidth: true
                    Label{
                        text: "吊数:"
                    }
                    SpinBox {
                        id:default_dropper_count
                        Layout.maximumWidth: 120
                        width: 40
                        height: 25
                        value: 5
                        from: 1
                        to: 12
                        editable: true
                        onValueChanged:{
                        }
                    }
                    Label{
                        text:"(杆之间的最大吊弦数)"
                    }

                    Rectangle{
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    RowLayout {
        id:buttons
        spacing: 10
        width: parent.width
        anchors.bottom: parent.bottom

        Rectangle{
            Layout.fillWidth: true
        }

        MyButton {
            id:btnNo
            text: qsTr("取消/关闭")
            onClicked: {
                requestCancel();
            }
        }

        MyButton {
            id:btnYes
            text: qsTr("确定保存")
            onClicked: {
                requestSave();
            }
        }

        Rectangle{
            Layout.fillWidth: true
        }
    }

    HScrollBar {
        anchors.right: flick.right
        anchors.rightMargin: -width
        anchors.top: flick.top
        height: flick.height
        width: 6*Devices.density
        color: cfg.mainColor
        forceVisible:true
        scrollArea: flick
    }
    InputDialog{
        id:inputDoubleDialog
        feildName:"请输入"
        autoCloseOnCancel:true
        width: parent.width * 0.8
        height: 200
        inputMethodHints: Qt.ImhDigitsOnly
        saveEnable : feildValue !== ""
        validator: DoubleValidator {
            bottom: -360
            top:  3000
        }
    }
}
