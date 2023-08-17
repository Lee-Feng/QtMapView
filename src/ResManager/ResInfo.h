#pragma once

#include <QObject>
#include <QImage>
#include <QMetaObject>

class ResInfo{
public:
    QString thum_file;  /// 缩略图
    QString src_file;   /// 原始连接
};
Q_DECLARE_METATYPE(ResInfo)
