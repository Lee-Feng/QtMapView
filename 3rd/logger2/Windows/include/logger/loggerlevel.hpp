/*******************************************************************************
* @file  loggerlevel.hpp
* @brief logger level description
*
* ...detail describe
*
* @version   V0.1.0
* @author    Jiangming
* @date      2013-12-17
*
* @note
*******************************************************************************/
#pragma once
#include "logger_config.hpp"

namespace utiles
{
class UTILES_LOGGER_DECL LogLevel
{
public:
    enum ELogLevel
    {
        E_Fatal  = 0,   /**< fatal error level */
        E_Error  = 300, /**< error level */
        E_Warn   = 400, /**< warnning level */
        E_Notice = 500, /**< notice level */
        E_Info   = 600, /**< infomation level */
        E_Debug  = 700, /**< debug level */
        E_NotSet = 800,
        E_Unknow = 900  /**< don't use */
    };
    typedef int Value;
};
}
