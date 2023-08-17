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

/// 目前用于 License V2.0.0
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

    if(license_dll_code != "ba4eb242211bdda29beb38610fba9bcc"){
        reason = QStringLiteral("无法校验Licensed,文件被篡改");
        return false;
    }
    if(license_d_dll_code != "2b3237c76a45af17d0972df3022724ff"){
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
