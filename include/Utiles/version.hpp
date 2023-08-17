#pragma once

#include <QString>
#include <QStringList>
#include <QDebug>

#ifndef _TO_STR

#define _TO_STR(S) #S
/*因为宏定义里有用'#'或'##'的地方宏参数是不会再展开,所以直接用_TOSTR的话如果A是宏不会被展开*/
#define TO_STR(S)        _TO_STR(S) /*这里S已经展开了，于是就可以用宏了*/

#endif


namespace utiles {


static inline int limit(int value,int max_value,int min_value){
    if(value > max_value){
        qWarning() << QStringLiteral("值[%1]大于了最大值[%2],进行截取").arg(value).arg(max_value);
        return max_value;
    }
    if(value < min_value){
        qWarning() << QStringLiteral("值[%1]小于了最小值[%2],进行截取").arg(value).arg(min_value);
        return min_value;
    }
    return value;
}

/// 版本号码由3部分组成。每个部分数值范围是0~999。
/// 每次更改，相关的号码需要至少加1
///
/// major版本号，变更为非兼容更新，大版本号代表非兼容的新增或者删除了接口。
/// minor子版本号，变更为兼容更新，大版本号代表兼容性的新增了字段或者接口。
/// update修订号，变更为兼容更新，大版本号代表修复了bug或者改动了内部实现。接口无任何修改。
///
inline static int make_version(int major,int minor,int update){
    major = limit(major,999,0);
    minor = limit(minor,999,0);
    update = limit(update,999,0);
    return update + minor * 1000 + major*1000000;
}

/**
 * @brief make_version
 * @param version
 * @return
 */
inline static int make_version(QString version){
    QStringList parts = version.split(".");
    while(parts.size() > 3){
        parts.removeLast();
    }
    int major = 0;
    int minor = 0;
    int update = 0;
    if(!parts.isEmpty()){
        major = parts.takeFirst().toInt();
    }
    if(!parts.isEmpty()){
        minor = parts.takeFirst().toInt();
    }
    if(!parts.isEmpty()){
        update = parts.takeFirst().toInt();
    }
    return make_version(major,minor,update);
}

inline static int get_verion_major(int version){
    return (version % 1000000000) / 1000000;
}
inline static int get_verion_minor(int version){
    return (version % 1000000)/ 1000;
}
inline static int get_verion_update(int version){
    return (version % 1000);
}

enum CompatibleTestError{
    Compatible_MajorError,
    Compatible_MinorError,
};

/**
 * @brief is_compatible
 * 检测是否兼容current_version版本
 * @param relative_version  这是程序设计时依赖的版本
 * @param current_version   这是新版本，需要检测是否当前程序能兼容该版本
 * @param can_ignore        是否能忽略将就用下
 * @return
 */
inline static bool is_compatible(int relative_version,int current_version,bool& can_ignore,CompatibleTestError& result,QString& reason){
    /// 主版本不同，肯定不能兼容，因为接口定义是不兼容变动
    if(get_verion_major(relative_version) != get_verion_major(current_version)){
        result = Compatible_MajorError;
        reason = QStringLiteral("主版本号不一致，接口不兼容");
        can_ignore = false;
        return false;
    }
    /// 如果当前版本的子版本低于要求的子版本，那么当前程序用到的数据或者接口可能没有，所以也无法兼容使用
    if(get_verion_minor(relative_version) > get_verion_minor(current_version)){
        result = Compatible_MinorError;
        reason = QStringLiteral("接口次版本号低于了依赖版本号，部分功能可能无法使用");
        can_ignore = true;
        return false;
    }
    return true;
}

/**
 * @brief get_version
 * 解析pro代码中定义的VERSION_STR值
 * 比如：DEFINES += VERSION_STR=$$VERSION
 * @param code  定义的字符串前3项
 * @param str 代码定义的字符串
 */
static void get_version(int& code,QString& str){
    QString versoin_str = TO_STR(VERSION_STR);
    if(versoin_str.isEmpty()){
        versoin_str = "1.0.0";
    }
    QStringList parts = versoin_str.split(".");

    int major = 0;
    int minor = 0;
    int update = 0;
    if(!parts.isEmpty()){
        major = parts.takeFirst().toInt();
    }
    if(!parts.isEmpty()){
        minor = parts.takeFirst().toInt();
    }
    if(!parts.isEmpty()){
        update = parts.takeFirst().toInt();
    }

    code = make_version(major,minor,update);
    str = versoin_str;
}

}
