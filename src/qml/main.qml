import QtQuick 2.9
import QtQuick.Controls 2.3
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0
import AsemanQml.Viewport 2.0
import AsemanQml.Sql 2.0
import QtQuick.Controls.Material 2.0
import AsemanQml.Awesome 2.0
import "qrc:/TaoQuick/BasicComponent/Progress/" as TaoQuick

AsemanApplication {
    id:app
    applicationAbout: cfg.company +cfg.productName + "客户端程序"
    applicationDisplayName: cfg.productName + "-" + cfg.company  + " " + cfg.versionString
    applicationVersion: cfg.versionString
    organizationDomain: "com.xdrailway"
    organizationName: "xdrailway team"

    property bool firstSetLocationShowed: false

    function convertFileUrl(filePath){
        if(filePath.startsWith("file://")){
            return filePath
        }

        if(filePath.startsWith("/")){
            return "file://" + filePath
        }
        return "file:///" + filePath
    }

    Config{
        id:cfg
    }

    Item{
        id:location

        property bool   gps_coord_valid: false
        property real   gps_latitude: 0
        property real   gps_longitude: 0
        property real   gps_altitude: 0

        property bool   bds_coord_valid: false
        property real   bds_latitude: 0
        property real   bds_longitude: 0
        property real   bds_altitude: 0

        /// 处理相机控制器信号
        Connections {
           target: $locationSys
           onGpsUpdated: {
               location.gps_coord_valid = isvalid
               location.gps_latitude = latitude
               location.gps_longitude = longtitude
               location.gps_altitude = altitude
           }
           onBdsUpdated: {
               location.bds_coord_valid = isvalid
               location.bds_latitude = latitude
               location.bds_longitude = longtitude
               location.bds_altitude = altitude
           }
        }

        Component.onCompleted: {

            var res = AsemanApp.checkPermissions(["android.permission.ACCESS_FINE_LOCATION"]);
            console.log("android.permission.ACCESS_FINE_LOCATION = " + res +  "("+ typeof(res) + ")");
            if (res === true) {
                console.log("获取到位置访问权限");
            } else {
                console.log("无位置访问权限");
            }

            $locationSys.setBdsEnabled(true)
            $locationSys.setGpsEnabled(true)
        }
        Component.onDestruction : {
            $locationSys.setBdsEnabled(false)
            $locationSys.setGpsEnabled(false)
        }
    }

    /// 设置信息
    Settings {
        id: settings
        source: $system.getAppStoragePath()  + "/meta/.settings.ini"
        category: "General"

        property alias backbone_name: settings.backboneNum

        /// 线路设置
        property string line_name: ""
        property string line_dir: ""
        property string station: ""
        property string tunnel: ""
        property string backboneNum: ""

        /// 其他设置
        property bool powerOffCameraWhenExit: true
    }

    AsemanWindow{
        visible: true
        id:appwindow
        width: 900
        height: 420
        /// 才可以全局管理返回按钮，否则部分QML自带的返回按钮无法拦截，比如Dialog
        backController: true
        /// 如果这里设置了，qtquickcontrols2.conf就屏蔽了
        /// 参考：https://doc.qt.io/qt-5/qtquickcontrols2-material.html
//        Material.primary: "#1E5391"
//        Material.accent: "#1E5391"
//        Material.theme: Material.Light
//        Material.foreground: "white"

        Viewport {
            id: viewport
            anchors.fill: parent
            mainItem: MainView{
            }
        }

        SplashImage{
            anchors.fill: parent
        }

        /// 全局toast,方便到处使用
        Toast{
            id:toast
        }
        Connections{
            target: $api
            onError:{
                toast.makeToast(error);
            }
        }

        ViewportController {
            viewport: viewport

            ViewportControllerRoute {
                route: /activity\:\/main.*/
                sourceComponent: MainView{
                }
            }
        }

        About{
            id:aboutDialog
            anchors.centerIn: parent
            width: parent.width*0.7
        }

        ExitDialog{
            id:exitDialog
            anchors.centerIn: parent
            width: parent.width*0.7

            Component.onCompleted: {
                handleBackOnce()
            }

            onClosed: {
                handleBackOnce()
            }

            function handleBackOnce(){
                BackHandler.pushHandler(viewport,function(){
                    exitDialog.open()
                })
            }
        }

        LocationSetting{
            id:settingDialog
            anchors.centerIn: parent
            width: parent.width*0.7
            miligateVisible:false

            line_name : settings.line_name
            line_dir : settings.line_dir
            station : settings.station
            tunnel : settings.tunnel
            backboneNum : settings.backboneNum

            onOpened: {
                let params = $api.getParams()
                settingDialog.line_name = params.location.line_name
                settingDialog.line_dir =  params.location.line_dir
                settingDialog.station =  params.location.station
                settingDialog.tunnel =  params.location.tunnel
                settingDialog.backboneNum =  params.location.back_bone
                settingDialog.backboneGap = params.backbone_interval
                settingDialog.forwordIsAcc = params.backbone_next_is_acc
                settingDialog.default_max_dropper_count = params.default_dropper_count
                settingDialog.mileage_revert_enabled = params.mileage_revert_enabled ? true : false
            }

            onRequestSave: {

                console.log("设置线路信息:" + line_name + "/" + line_dir + "/" + station + "/" + tunnel + "/" + backboneNum);
                var loc = {}
                loc.line_name = line_name
                loc.line_dir = line_dir
                loc.station = station
                loc.tunnel = tunnel
                loc.back_bone = backboneNum
                var params = {}
                params.location = loc
                params.backbone_interval = backboneGap
                params.backbone_next_is_acc = forwordIsAcc
                params.default_dropper_count = default_max_dropper_count;
                params.mileage_revert_enabled = mileage_revert_enabled
                $api.setParams(params)
                close()
            }

            onRequestCancel: {
                close()
            }
        }
    }
}
