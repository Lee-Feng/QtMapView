/*******************************************************************************
* @file  logger_appender.hpp
* @brief
*
* use log4cpp or other logger library to implement our own LOGGER_USE_LOG4CPP
*
* @version   V0.1.0
* @author    Jiangming
* @date      2013-12-17
*
* @note
*******************************************************************************/
#ifndef _LOGGERAPPENDER_HPP_
#define _LOGGERAPPENDER_HPP_
#include <string>

namespace utiles
{

class LoggerAppender
{
public:
    virtual ~LoggerAppender(){;}
    virtual void Append(const std::string& logtext) = 0;
};


}//end namespace utiles
#endif
