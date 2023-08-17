#pragma once;
#include <qglobal.h>
#include <License/LicenseConfig.hpp>
#include <QDateTime>
#include <License/AuthData.h>

/**
 * @brief The License class
 *
 * 支持多线程访问。线程安全！
 *
 * 授权检测原理：
 *  1）授权码的生成：当前机器人信息(长度不固定)--->hash计算-->机器hash码(固定长度)--->附加授权信息-->AES加密-->LicenseCode
 *  2）授权码的有效性检测:LicenseCode-->AES解密-->hash码+授权信息
 *                    当前机器人信息(长度不固定)--->hash计算机器hash码(固定长度)
 *                    对比hash值是否一致。一致了再检测授权信息的内容。
 *  3）最终授权文件的内容
 *       {
 *           "hash_code": "fhafglkslfgslkgjlkdsjlkf",
 *           "generate_date":"2018-11-12 15:42:20,098",
 *           "version":"1.0.0",
 *           "program_limit":1,
 *           "program":"F:\FDRSoft\JiuZhouAssemblyLineClient1.0.0\SCADAClient_d.exe",
 *           "program_hash":"fsfdsgdgdg",
 *           "auth_data": [{
 *               "auth_type": "***",
 *               "auth_content":{
 *               }
 *           }]
 *
 *       }
 *
 *      文件内容讲解：
 *       hash_code:就是系统的硬件信息
 *       generate_date：就是该文件内容的生成时间
 *       version：就是该授权文件的版本号
 *       program：就是授权赋予的程序文件，只有该程序文件启动才会被授权，更换了程序授权也会丢失
 *       program_hash:是赋予的授权程序文件的hash值。如果文件更改了，那么授权也会丢失。
 *       auth_data：是额外的授权信息，比如时间限制，次数限制等。
 *               这是个数组，每一项就叫一个授权节点，只有每个节点的授权都满足程序才能使用。
 *               auth_data如果是空，将代表不能进行任何授权，可被当做请求文件，根据该请求文件+授权的配置(auth_data)，那么就可以生成完整的首选文件
 * 授权机制说明：
 *  License模块会生成一个license/license.db3数据库，里面记录程序的启动运行信息。
 *  模块启动时会解析数据库，以及加载License目录的相关lecense文件(后缀是getLicenseFileExtention的返回值)。
 *  一个授权文件列表对应一个exe程序。所以license模块会检测当前运行的exe路径，并查找对应的授权文件进行授权结论检测。
 *  如果授权文件是空(空授权文件)，那么该文件就相当于一个请求文件的功能。
 *
 * 目录结构：
 *  my_app
 *      /--log
 *      /--license
 *          /--license.db3
 *          /--授权文件1.lic
 *          /--授权文件2.lic
 *          /--授权文件3.lic
 *          /--授权文件4.lic
 *          /--授权实现库1.dll
 *          /--授权实现库2.dll
 *          /--授权实现库3.dll
 *          /--授权实现库4.dll
 *
 */
class LF_LICENSE_DECL License
{
public:
    enum HashContent{
        HC_CPU          = 0x01, /// CPU序列号
        HC_BOARD        = 0x02, /// 主板ID
        HC_SYSTEMDISK   = 0x04, /// 系统所在硬盘的序列号
        HC_BIOS         = 0x08, /// BIOS序列号
        HC_MAC          = 0x10, /// MAC地址
        HC_GENERAL      = (HC_CPU | HC_MAC), /// 通常使用的方式
        HC_ALL          = 0xFF, /// 所有信息
    };
public:

    /**
     * @brief initPassword
     * 将触发重新加载文件
     * @param password
     */
    static void initPassword(QString password);

    /**
     * @brief License
     * @param password 文件的加密解密密码，用于授权检测。
     * @param license_files_root  license的文件根目录，比如"./license_files"
     * @param license_files_root  license的库的根目录，比如"./license"
     *  注意同一个目录下面的授权文件应该使用一样的密码。
     */
    License(const QString& license_files_root, const QString& license_lib_root);
    virtual ~License();

    /**
     * @brief getLicense
     * 根据指定的目录构造license
     * @param dir
     * @return
     */
    static License* getLicense(const QString& license_files_root,const QString& license_lib_root);

    /**
     * @brief getGlobalLicense
     * 获取全局的License
     * @return
     */
    static License* getGlobalLicense();
public:
    /**
     * @brief getSupportedAuthItems
     * 获取当前系统支持的所有授权信息.如果还没初始化完成会等待初始化完成。
     * 只读的Item，用于基本信息获取。如果要使用该Item，请调用newAuthItem获得。
     * @return
     */
    QList<license::IConstAuthItemPtr> getSupportedAuthItems() const;

    /**
     * @brief getAuthItem
     * 获取授权的Item,只读的Item，用于基本信息获取。如果要使用该Item，请调用newAuthItem获得。
     * @param auth_type
     * @return
     */
    license::IConstAuthItemPtr getAuthItem(const QString& auth_type) const;

    /**
     * @brief newAuthItem
     * 够一个新授权对象，如果没有返回null
     * @param auth_type
     * @return
     */
    license::IAuthItemPtr newAuthItem(const QString& auth_type) const;

public:
    /**
     * @brief getVersion
     * 获取版本号
     * @return
     */
    QString getVersion() const;

    /**
     * @brief getLicenseFileExtention
     * 获取license文件的后缀名。
     * lic文件与.req文件的格式是相同的。用不同后缀只是为了区分文件的职能。
     * @return
     */
    QString getLicenseFileExtention() const {return ".lic";}

    /**
     * @brief getRequestFileExtention
     * 获取请求文件的后缀名
     * lic文件与.req文件的格式是相同的。用不同后缀只是为了区分文件的职能。
     * @return
     */
    QString getRequestFileExtention() const {return ".req";}

    /**
     * @brief getMachineCode
     * 获取当前主机的硬件值，不同的主机得到的数据肯定不一样
     * 里面涵盖硬件信息：网卡，cpu等信息
     * @param hc 参数指定获取该信息的内容。如果是多个将以换行符"\n"分割
     * @return
     */
    QString getMachineCode(HashContent hc,QString sep = "\n") const;

    /**
     * @brief getHashCode
     * 获取机器人信息的hash值。信息的长度固定。
     * @param hc
     * @return
     */
    QString getHashCode(HashContent hc = HC_GENERAL) const;

    /**
     * @brief calcFileHash
     * 计算文件hash
     * @param filepath
     * @param hash
     * @return
     */
    bool calcFileHash(const QString& filepath,QString& hash) const;

    /**
     * @brief generateRequestCode
     * 等同于generateLicenseCode(request,QByteArray());
     * @return 内容，已加密,密码固定是123456
     */
    QByteArray generateRequestCode(const license::RequestInfo& request) const;

    /**
     * @brief generateLicenseCode
     * 根据指定的hashcode生成一个对应的授权码内容。
     * 其实就是hash_code对用的一个加密字符串，但是每次得到的字符串内容不一样。
     * 加密密码由setDefaultPassword指定。
     * @param request   机器人请求信息
     * @param auth_data 附带的授权信息，可以是任意信息，通常包含了授权文件的内容，比如是试用7天还是几个小时，还是启动几次等等限制。
     *                  如果auth_data为空，那么该license文件只能作为request文件。不能作为有效的授权文件。
     * @return 授权内容，已加密
     */
    QByteArray generateLicenseCode(const license::LicenseFile& file) const;

    /**
     * @brief parseLicense
     * 根据licenseCode计算出对应的hash值+授权信息。
     * 其实就是解密过程，解密密码由setDefaultPassword指定。
     * 使用方式：
     *      license::LicenseFile file;
     *      parseLicense(licenseCode,file,error);
     *
     * @param licenseCode
     * @param file          解析得到的文件内容。
     * @param error         解析失败的原因
     * @return 只有解密成功才能获取
     */
    bool parseLicense(const QByteArray& licenseCode,license::LicenseFile &file, QString& error) const;

    /**
     * @brief getCurrentProgramRunTimeInfo
     * 获取程序的当前运行信息
     * @return
     */
    const license::ProgramRunTimeInfo& getCurrentProgramRunTimeInfo() const;

    /**
     * @brief licenseValid
     * 检测当前的授权文件是否可用。
     * 根据自己的hash值等信息与授权文件的值进行解密计算对比，
     * 如果一致就代表授权成功，否则失败。
     * @param data_template 授权模板
     * @param reason        授权失败的原因
     * @return
     */
    bool licenseValid(QString& reason) const;

private:
    License(License&);
    License& operator = (License& s);
    void loadFiles();
private:
    class Impl;
    Impl* m_Impl;
    friend class LicenseInstancePool;

};


