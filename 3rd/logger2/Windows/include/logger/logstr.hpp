/*******************************************************************************
* @file .hpp
* @brief  
*
* ...detail describe
*
* @version   V1.0.0
* @author    Jiangming
* @date      2017-03-10
*
* @note
*******************************************************************************/
#ifndef LOGGER_LOGSTR_HPP_
#define LOGGER_LOGSTR_HPP_


//将utf-8转换成gb2312
#define Logstr(str)\
    str.toLocal8Bit().toStdString()
#endif // LOGGER_LOGSTR_HPP_
