#pragma once

#include <QtGlobal>
#include <Utiles/ByteOrder.hpp>
#include <QtEndian>

namespace utiles {

namespace private_data {
#pragma pack(push)//保存对齐状态
#pragma pack(1)   //设定为1字节对齐
struct Float
{
    quint16 low;
    quint16 high;
};
#pragma pack(pop) //保存对齐状态

}


/**
 * @brief toReal
 * 2个short拼接为float
 * @param low
 * @param high
 * @return
 */
static inline float toReal(quint16 low,quint16 high,ByteOrder bo = LittleEndian){
    static_assert(sizeof(float) == sizeof(private_data::Float),"Float size is not equal float size");

    if(bo == LittleEndian){
        /// 转为小端，这样内存拼接在一起的结果才是float的内存模型
        /// 低地址->高地址
        /// low   |high
        /// [0][1]|[2][3]
        low = qToLittleEndian(low);
        high = qToLittleEndian(high);
    } else {
        low = qToBigEndian(low);
        high = qToBigEndian(high);
    }

    private_data::Float f;
    f.low = low;
    f.high = high;
    float ret = *((float*)&f);
    return ret;
}

/**
 * @brief splitReal
 * float拆分为2个short
 * @param v
 * @param low
 * @param high
 */
static inline void splitReal(float v,quint16& low,quint16& high,ByteOrder bo = LittleEndian){
    static_assert(sizeof(float) == sizeof(private_data::Float),"Float size is not equal float size");
    private_data::Float *f = (private_data::Float*)(&v);
    low = f->low;
    high = f->high;

    if(bo == LittleEndian){
        /// float数据想当于小端，所以需要当小端处理。
        /// 低地址->高地址
        /// low   |high
        /// [0][1]|[2][3]
        low = qFromLittleEndian(low);
        high = qFromLittleEndian(high);
    } else {
        low = qFromBigEndian(low);
        high = qFromBigEndian(high);
    }
}

}
