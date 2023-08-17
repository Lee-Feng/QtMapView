/*******************************************************************************
* @file   .hpp
* @brief
*
* 参数构建，解析类
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2016-05-12
*
* @note
*******************************************************************************/
#pragma once

#include <License/License.h>
#include <QString>

namespace utiles {

static bool LicenseCheck(QString& reason,QString callMe = ""){
    if(callMe.isEmpty()){
        callMe = QStringLiteral("软件开发商");
    }
    QString license_dll_code;
    if(!License::getGlobalLicense()->calcFileHash("License.dll",license_dll_code)){
        reason = QStringLiteral("无法校验License");
        return false;
    }
    QString license_d_dll_code;
    if(!License::getGlobalLicense()->calcFileHash("License_d.dll",license_d_dll_code)){
        reason = QStringLiteral("无法校验Licensed");
        return false;
    }

    if(license_dll_code != "12d3312908344b4fe70ac6f76a804e3f"){
        reason = QStringLiteral("无法校验Licensed,文件被篡改");
        return false;
    }
    if(license_d_dll_code != "ee0f6bed1bbcd040686e7b559ce64ed1"){
        reason = QStringLiteral("无法校验Licensed,文件被篡改");
        return false;
    }

    if(!License::getGlobalLicense()->licenseValid(reason)){
        reason = QStringLiteral("程序授权未通过,请联系%1").arg(callMe);
        return false;
    }
    return true;
}


}
