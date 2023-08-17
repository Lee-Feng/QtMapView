/*******************************************************************************
* @file logcov.hpp
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
#ifndef _FDR_LOGCOV_HPP_
#define _FDR_LOGCOV_HPP_
#include <boost/locale/encoding.hpp>

//将utf-8转换成gb2312
#define trlog(str)\
    boost::locale::conv::between(str, "GBK", "UTF-8")
#endif // _FDR_LOGCOV_HPP_
