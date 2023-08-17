#pragma once
#include <QString>
typedef struct
{
    int id;
    QString station;            //站区名称
    QString tunnel;             //隧道名称
    QString poleID;             //支柱号
    float poleDist;             //跨距
    int structFlag;             //结构标志
    int curveFlag;              //取消标志
    QString distFlag;           //公里标
    QString powerSupply;        //供电段
    QString factory;            //车间名称
    QString workArea;           //工区
    int wireHight;              //导高标准值
    int offset;                 //拉出值标准值
} LineBaseDataItem;
/// 兼容赵老师的基础数据设计
typedef LineBaseDataItem oneLineData_t;
