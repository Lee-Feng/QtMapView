/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2020-06-10
*
* @note
*******************************************************************************/

#pragma once

#include <QString>
#include <QStringList>
#include <QDebug>

namespace utiles {

/**
 * @brief The SmartSpilt class
 * 智能分割。用于分割文字和字符串。
 */
class SmartSplit{
public:
    enum SplitMode{
        /// 文字和正数
        /// 比如 "测试文字001"    --> "测试文字" + "001"
        /// 比如 "测试文字-001"   --> "测试文字-" + "001"
        /// 比如 "测试文字1"      --> "测试文字" + "1"
        /// 比如 "测试文字"       --> "测试文字" + ""
        /// 比如 "001测试文字"    --> "001测试文字" + ""
        Mode_Text_PositiveNum = 0,
    };

public:
    /**
     * @brief splitPositiveNum
     * 拆分正数
     * @param txt
     * @param base
     * @param num
     */
    void splitPositiveNum(const QString& txt,QString& base,QString& num){
        QStringList data = split(txt,Mode_Text_PositiveNum);
        if(!data.isEmpty()){
            base = data.takeFirst();
        }
        if(!data.isEmpty()){
            num = data.takeFirst();
        }
    }

    /**
     * @brief incPositiveNumPart
     * 尾部正数部分数字增加
     * @param txt       文字，比如“测试文字-001”
     * @param incValue  增加值，如果是负数就递减
     * @return
     */
    QString incPositiveNumPart(const QString& txt,int incValue,int fieldWidth=0,QChar fillChar='0'){
        QString base;
        QString num;
        splitPositiveNum(txt,base,num);
        int value = num.toInt();
        value += incValue;
        if(value <= 0){
            value = 1;
        }
        return QString("%1%2").arg(base).arg(value,fieldWidth,10,fillChar);
    }
    QString formatPositiveNumPart(const QString& txt,int fieldWidth=0,QChar fillChar='0'){
        QString base;
        QString num;
        splitPositiveNum(txt,base,num);
        int value = num.toInt();
        return QString("%1%2").arg(base).arg(value,fieldWidth,10,fillChar);
    }

    /**
     * @brief split
     * 拆分
     * @param txt
     * @param mode
     * @return
     */
    QStringList split(const QString& txt,SplitMode mode){
        QStringList ret;
        switch (mode) {
        case Mode_Text_PositiveNum:
            ret = splitTextAndPositiveNum(txt);
            break;
        default:
            qWarning() << QString("unsupported split mode(%1)").arg(mode);
            /// 不支持
            ret.append(txt);
            break;
        }
        return ret;
    }
protected:
    /// 从后往前前处理，只找结尾部分是数字的情况
    QStringList splitTextAndPositiveNum(QString txt){
        txt = txt.trimmed();
        int head_pos = -1;
        for(int i=txt.size()-1; i>=0; i--){
            if(txt[i] >= '0' && txt[i] <= '9'){
                head_pos = i;
            } else {
                break;
            }
        }
        QStringList ret;
        ret.append(txt.mid(0,head_pos));
        ret.append(txt.mid(head_pos,-1));
        return ret;
    }
};

}
