import QtQuick 2.9
import QtQuick.Controls 2.3
import Qt.labs.folderlistmodel 2.2
import ImageItem 1.0
import QtQuick.Controls.Material 2.12

Item {
    id: root

    // 20221120版本：
    property string versionString: "V0.0.1.20221120"
    property string discribe: "设备用于XXXXX功能.."

    /// 资源排列顺序
    property var sortField: FolderListModel.Time
    /// false:最新的在最前面
    /// true:最新的在最后面
    property bool sortReversed: false

    /// 录像功能是否开启，目前录像完成之后需要下载比较麻烦
    property bool feature_vedio_enable: false

    /// 公司全称
    property string company: "XXXX公司"
    /// 公司简写
    property string company_short: "XX"
    /// 产品名字
    property string productName: "XXX仪"

    /// 定义颜色，方便统一
    property color mainColor: appwindow.Material.primary
    /// 基于mainColor的文字颜色
    property color textColor: "white"


    /// 显示坐标调试界面
    property bool showLocationDebug: true

    /// 相机实时画面填充方式
    property var cameraViewFillMode: ImageItem.PreserveAspectCrop

    /// 相机界面左侧操作栏宽度
    property int operateAreaWidth: 320

    /// 发现端口
    property int discovery_port: 8990

    /// 精度：小数点之后的有效位数
    property int precision_counts: 2

    /// 运动到准备点的速度
    property double moveToReadyPosVel: 99
    property double jogMoveMaxSpeed: 80

    /// 按钮高度
    property int button_height: 45
}
