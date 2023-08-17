#pragma once;
#include <halconcpp/HalconCpp.h>
#include <QString>

namespace utiles {


static inline QString ParseHalconException(const HalconCpp::HException &e, const QString &ed)
{
    QString des = QStringLiteral("%1. halcon算子抛出异常。异常信息，程序名字：%2，错误号：%3，错误信息：%4")
            .arg(ed)
            .arg(e.ProcName().Text()).arg(e.ErrorCode()).arg(e.ErrorMessage().Text());
    return des;
}




}




