import QtQuick 2.9
import QtQuick.Controls 2.3
import AsemanQml.Base 2.0
import AsemanQml.Controls 2.0

Label{
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 1
    font.pixelSize: Math.floor(9*Devices.fontDensity)
    color:"white"
    text: {
        return settings.line_name + "/" +
                settings.line_dir + "/" +
                settings.station + "/"  +
                settings.tunnel + "/杆号:" +
                settings.backboneNum
    }
}
