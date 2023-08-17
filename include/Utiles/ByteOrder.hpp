#pragma once

#include <QtGlobal>

namespace utiles {

namespace private_data {

/// 是指把原来对齐方式设置压栈，并设新的对齐方式设置为一个字节对齐
#pragma  pack (push,1)
struct Short
{
    quint8 byte0;
    quint8 byte1;
};
/// 恢复对齐状态
#pragma pack(pop)

}

enum ByteOrder{
    LittleEndian = 0,       /// 小段模式
    BigEndian = 1,          /// 大段模式，网络字节序
};

/**
 * @brief thisByteOrder
 * 返回当前的字节序
 * @return
 */
static inline ByteOrder thisByteOrder(){
    static_assert(sizeof(private_data::Short) == sizeof(quint16),"Data memory size error");
    quint16 test = 0x2211;
    private_data::Short* s = ((private_data::Short*)&test);
    /// 值1存在低地址就是小段
    if(s->byte0 == 0x11 && s->byte1 == 0x22){
        return LittleEndian;
    } else  if(s->byte0 == 0x22 && s->byte1 == 0x11){
        return BigEndian;
    }
    Q_ASSERT(false);
    return BigEndian;
}

/**
 * @brief byteOrderToString
 * @param bo
 * @return
 */
static inline const char* byteOrderToString(ByteOrder bo){
    switch (bo) {
    case LittleEndian:
        return "LittleEndian";
        break;
    case BigEndian:
        return "BigEndian";
        break;
    default:
        return "Unknown";
        break;
    }
}

}
