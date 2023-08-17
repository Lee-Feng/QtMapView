/*******************************************************************************
* @file logger.h
* @brief logger
*
* ...detail describe
*
* @version   V1.0.0
* @author    Jiangming
* @date      2013-12-17
*
* @note
*******************************************************************************/
#pragma once

#include "logger_config.hpp"
#include "loggerlevel.hpp"
#include <string>
#include <memory>

namespace utiles
{

/**
 * @brief CLogger
 *
 * Usage:
 * the first step is create the logger instance by using
 * GetNamedLogger(yourloggername) or GetLogger() in global namespace .
 *
 * The @see CLogger::In may be added befor the logging code worked
 * when you need to add the name of who logged the log information
 * as follows:
 *      CLogger* rootlogger = GetLogger();
 *      CLogger::In module("mainmodule", rootlogger);
 *      rootlogger->Error("the is some thing wrong in main module");
 *
 * the log result in log file will be like this:
 * 2014-01-28 10:53:20,476: [ERROR] [mainmodule] : the is some thing wrong in main module
 *
 * the name of the log file be genreated when create the logger instance,
 * with the form of "log_$time$_$loggername$.txt" ,such as:"log_105320_root.txt"
 * and the file will be located in the path of"./log/$date$" such as :./log/2014-Jan-28/
 *
 */
class UTILES_LOGGER_DECL CLogger
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
    typedef std::shared_ptr<CLogger> LoggerPtr;

    class In
    {
    public:
        UTILES_LOGGER_DECL In(const std::string& module, LoggerPtr plogger);
        UTILES_LOGGER_DECL ~In();
        std::string oldmod;
        LoggerPtr m_plogger;
    };

    static LoggerPtr GetRootInstance();
    static LoggerPtr GetInstance(const std::string& loggername);
    /**
     * @brief write log
     * @param[in] e_level :log level
     * @param[in] msg :log infomation
     */
    void WriteLog(ELogLevel e_level, const std::string& msg);


    void Log(LogLevel::Value level, const std::string& msg) throw();

    /**
     * log message
     *
     * @param[in] module .the module name which log this message
     * @param[in] level. log level .notice: message only higher or equal to
     *                   the set level will be log.
     * @param[in] msg .message to be log.
     **/
    void Log(const std::string& module, LogLevel::Value level,
             const std::string& msg) throw();
    /**
     * Log a message with the specified log level.
     * @param level The level of this log message.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Log_f(LogLevel::Value level, const char* stringFormat, ...) throw();

    /**
     * Log a message with debug level.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Debug_f(const char* stringFormat, ...) throw();

    /**
     * Log a message with debug level.
     * @param message string to write in the log file
     **/
    void Debug(const std::string& message) throw();

    /**
     * Log a message with info level.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Info_f(const char* stringFormat, ...) throw();

    /**
     * Log a message with info level.
     * @param message string to write in the log file
     **/
    void Info(const std::string& message) throw();

    /**
     * Log a message with notice level.
     * @param message string to write in the log file
     **/
    void Notice(const std::string& message) throw();

    /**
     * Log a message with warn level.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Warn_f(const char* stringFormat, ...) throw();


    /**
     * Log a message with warn level.
     * @param message string to write in the log file
     **/
    void Warn(const std::string& message) throw();

    /**
     * Log a message with error level.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Error_f(const char* stringFormat, ...) throw();

    /**
     * Log a message with error level.
     * @param message string to write in the log file
     **/
    void Error(const std::string& message) throw();

    /**
     * Log a message with fatal level.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void Fatal_f(const char* stringFormat, ...) throw();

    /**
     * Log a message with fatal level.
     * @param message string to write in the log file
     **/
    void Fatal(const std::string& message) throw();

    /**
     * set loglevel of logging message
     * all messages with this or higher importance will be log
     *
     **/
    void SetLogLevel(LogLevel::Value level);

    /**
     * check is the specify log level is enable.
     * if enable all messages with this or higher importance will be log
     **/
    bool IsLevelEnable(LogLevel::Value level);
     ~CLogger();
private:
    /**
    * Inform the Logger of the entry of a module.
    * @see In. Use Logger::In(\a modname) for management.
    */
    void in(const std::string& modname);

    /**
    * The counterpart of in().
    * @see In. Use Logger::In(\a modname) for management.
    */
    void out(const std::string& modname);

    /**
    * Get the name of the current Log generating Module.
    */
    std::string getLogModule() const;

    friend class In;
    friend class LoggerMain;
private:
    CLogger();
    CLogger(const std::string& loggername);
    class Impl;
    Impl* m_pimpl;

public:
    void AddAppender(void*, bool customappender = true);
    void RemoveAppender(void*, bool customappender = true);
};
}

typedef utiles::CLogger::LoggerPtr utiles_LoggerPtr;
#define GetLogger() CLogger::GetRootInstance()
#define GetNamedLogger(name) CLogger::GetInstance(name)
#define LogRoot utiles::CLogger::GetRootInstance()
