#pragma once

namespace utiles {

/**
 * @brief to_soap_time
 * @param time_ms   毫秒
 * @return gsoap时间，小于0代表微妙，大于0代表秒
 */
static int to_soap_time(int time_ms){
    if(time_ms < 1000){
        return -(time_ms * 1000);
    }
    return time_ms/1000;
}

}
