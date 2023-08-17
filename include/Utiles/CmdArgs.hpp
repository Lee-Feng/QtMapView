/*******************************************************************************
*
* @file   .hpp
* @brief
*
* 参数构建，解析类
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2016-05-12
*
* @note
*******************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <assert.h>

#ifndef TOSTR
#define TOSTR(STR)  #STR
#endif
#ifndef STR
#define STR(R)      TOSTR(R)
#endif

namespace utiles {

/**
 * @brief The CmdArgBuilder class
 * 参数构建器，用于构建参数列表
 */
class CmdArgBuilder{

    /// 参数类型
    enum ArgType{
        AT_NoValue,
        AT_BoolValue,
        AT_IntValue,
        AT_FloatValue,
        AT_StringValue,
        AT_StringListValue,  /// 比如参数-s是AT_StringListValue类型，那么-s -a -b -c便都是直接把-a -b -c提取出来,该类型的参数只能放在最后。
    };

    /// 参数名字定义
    class Arg{
    public:
        std::string name;
        std::string alias;
        std::string comments;

        Arg(int arg_id){
            m_argid = arg_id;
        }

        int getId() const{
            return m_argid;
        }

        bool operator ==(const Arg& t){
            return t.m_argid == this->m_argid;
        }

    private:
        int m_argid;
    };

    /// 参数值
    class ArgValue{
    public:
        ArgType     type;
        union{
            bool*        boolValue;   /// 如果是noValue,该值用于描述参数是否存在
            int*         intValue;
            float*       floatValue;
            std::string* stringValue;
            std::vector<std::string>* stringlistValue;
        }v;

        ArgValue(){
            type = AT_NoValue;
            v.boolValue = NULL;
        }

        /// 指定是bool值还是没有值
        ArgValue(bool* dst_value,bool default_value,bool is_no_value = true){
            type = is_no_value ? AT_NoValue : AT_BoolValue;
            v.boolValue = dst_value;
            if(v.boolValue != NULL){
                *v.boolValue = default_value;
            }
        }

        ArgValue(int* dst_value,int default_value){
            type = AT_IntValue;
            v.intValue = dst_value;
            if(v.intValue != NULL){
                *v.intValue = default_value;
            }
        }
        ArgValue(float* dst_value,float default_value){
            type = AT_FloatValue;
            v.floatValue = dst_value;
            if(v.floatValue != NULL){
                *v.floatValue = default_value;
            }
        }
        ArgValue(std::string* dst_value,std::string default_value){
            type = AT_StringValue;
            v.stringValue = dst_value;
            if(v.stringValue != NULL){
                *v.stringValue = default_value;
            }
        }
        ArgValue(std::vector<std::string>* dst_value,std::vector<std::string> default_value){
            type = AT_StringListValue;
            v.stringlistValue = dst_value;
            if(v.stringlistValue != NULL){
                *v.stringlistValue = default_value;
            }
        }

        /// 用于AT_NoValue
        void setContains(bool contains){
            setValue(contains);
        }

        /// 用于AT_BoolValue，AT_NoValue
        void setValue(bool value){
            if( ( type == AT_BoolValue || type == AT_NoValue)  && v.boolValue != NULL){
                *(v.boolValue) = value;
            }
        }

        /// 用于AT_IntValue
        void setValue(int value){
            if(type == AT_IntValue && v.intValue != NULL){
                *(v.intValue) = value;
            }
        }

        /// 用于AT_FloatValue
        void setValue(float value){
            if(type == AT_FloatValue && v.floatValue != NULL){
                *(v.floatValue) = value;
            }
        }

        /// 用于AT_StringValue
        void setValue(const std::string& value){
            if(type == AT_StringValue && v.stringValue != NULL){
                *(v.stringValue) = value;
            }
        }

        /// 用于AT_StringListValue
        void setValue(const std::vector<std::string>& value){
            if(type == AT_StringListValue && v.stringlistValue != NULL){
                *(v.stringlistValue) = value;
            }
        }
    };

    class ArgValuePair{
    public:
        Arg arg;
        ArgValue value;
        ArgValuePair():arg(-1){
        }
        ArgValuePair(const Arg& _arg):arg(_arg){
        }
        ArgValuePair(const Arg& _arg,const ArgValue& _v):arg(_arg),value(_v){
        }
    };

public:
    /**
     * @brief CmdArgBuilder
     * @param name_frefix           长名字的前缀，比如--test 前缀就是--
     * @param alias_name_frefix     短名字前缀，比如-t,前缀就是-
     */
    CmdArgBuilder(const std::string& name_frefix = "--",const std::string& alias_name_frefix="-"){
        m_name_frefix = name_frefix;
        m_alias_name_frefix = alias_name_frefix;
        m_help = false;
    }

    /**
     * @brief addHelpArg
     * 添加帮助参数
     * @param name
     * @param alias
     * @return
     */
    bool addHelpArg(const std::string& name = "help",const std::string& alias = "h"){
        return addArg(name,alias,"查看帮助信息",m_help,m_help);
    }

    /**
     * @brief addArg
     * 添加一个参数。
     * 比如addArg("test"),addArg("test","-t")
     * 这样启动参数就可以使用：
     * test.exe -test 或者test.exe -t
     * @param name      参数名,不包括前缀(比如-,或者--)     不能重复。
     * @param alias     参数简写,不包括前缀(比如-,或者--)    不能重复。
     * @param comments  参数描述
     * @param contains  用于记录该参数是否存在,如果存在或者default_contains是true，就是true
     * @param default_contains  如果参数不存在使用的默认值
     */
    bool addArg(const std::string& name,const std::string& alias,const std::string& comments,bool& contains,bool default_contains = false){
        Arg a(getNextArgId());
        a.name = m_name_frefix + name;
        a.alias = m_alias_name_frefix + alias;
        a.comments = comments;
        ArgValue v(&contains,default_contains,true);
        return addOneArg(a,v);
    }

    /**
     * @brief addValueArg
     * 添加一个获取指定类型值的参数.
     * 例如：
     * int value;
     * add("int","i","传入整形数字",value)
     * @param name
     * @param alias
     * @param comments
     * @param value             目前支持的类型：bool,int,float,std::string
     * @param defaul_value      默认值目前支持的类型：bool,int,float,std::string
     */
    template<typename T>
    bool addValueArg(const std::string& name,const std::string& alias,const std::string& comments,T& value,const T& defaul_value){
        Arg a(getNextArgId());
        a.name = m_name_frefix + name;
        a.alias = m_alias_name_frefix + alias;
        a.comments = comments;
        ArgValue v(&value,defaul_value);
        return addOneArg(a,v);
    }

    bool getArg(const std::string& name_or_alias,Arg& arg) const {
        std::map<int,ArgValuePair>::const_iterator it = m_args.cbegin();
        for(; it != m_args.cend() ; it++){
            if(it->second.arg.name == name_or_alias || it->second.arg.alias == name_or_alias){
                arg = it->second.arg;
                return true;
            }
        }
        return false;
    }

    void setHeader(const std::string& h){
        m_header = h;
    }
    void setFooter(const std::string& f){
        m_footer = f;
    }

public:
    /**
     * @brief parse
     * 解析参数值，并把结果存放到Arg的值中。
     * 如果有请求帮助文档，请调用requestHelp检测。
     * @param args
     * @param error
     * @return
     */
    bool parse(const std::vector<std::string>& args,std::string& error){
        m_help = false;

        bool ret = true;
        for(int i=0,cnt=args.size(); i<cnt; i++){
            ArgValuePair ap;
            if(!getArgValuePair(args[i],ap)){
                continue;
            }
            bool is_error = false;
            std::stringstream  os;
            int intValue = 0;
            bool boolValue = false;
            float floatValue = 0;
            std::vector<std::string> listvalue;

            switch (ap.value.type) {
            case AT_NoValue:
                ap.value.setContains(true);
                break;
            case AT_BoolValue:
                if(i + 1 >= cnt){
                    is_error = true;
                    os << "参数" << args[i] << "后面缺少参数,应该跟BOOL标记量";
                    error = os.str();
                    break;
                }
                if(args[i+1] == "true" || args[i+1] == "TRUE"){
                    boolValue = true;
                } else if(args[i+1] == "false" || args[i+1] == "FALSE"){
                    boolValue = false;
                } else {
                    os << args[i+1];
                    os >> intValue;
                    boolValue = (intValue != 0);
                }
                ap.value.setValue(boolValue);
                break;
            case AT_IntValue:
                if(i + 1 >= cnt){
                    is_error = true;
                    os << "参数" << args[i] << "后面缺少参数,应该跟INT数字";
                    error = os.str();
                    break;
                }
                os << args[i+1];
                os >> intValue;
                ap.value.setValue(intValue);
                break;
            case AT_FloatValue:
                if(i + 1 >= cnt){
                    is_error = true;
                    os << "参数" << args[i] << "后面缺少参数,应该跟FLOAT数字";
                    error = os.str();
                    break;
                }
                os << args[i+1];
                os >> floatValue;
                ap.value.setValue(floatValue);
                break;
            case AT_StringValue:
                if(i + 1 >= cnt){
                    is_error = true;
                    os << "参数" << args[i] << "后面缺少参数,应该跟字符串";
                    error = os.str();
                    break;
                }
                ap.value.setValue(args[i+1]);
                break;
            case AT_StringListValue:
                listvalue.clear();
                while(i + 1 < cnt){
                    listvalue.push_back(args[i+1]);
                    i++;
                }
                ap.value.setValue(listvalue);
                break;
            default:
                is_error = true;
                os << "不支持的参数值类型<" << ap.value.type << ">";
                error = os.str();
                assert(false);
                break;
            }

            if(is_error){
                ret = false;
                break;
            }
        }
        return ret;
    }

    /**
     * @brief requestHelp
     * 是否有请求帮助文档。
     * parse接口调用之后可用。
     * @return
     */
    bool requestHelp(){
        return m_help;
    }

    /**
     * @brief help
     * 获取帮助信息
     * @return
     */
    std::string help() const{
        std::ostringstream os;
        if(!m_header.empty()){
            os << m_header << "\n";
        }
        os << "参数列表:\n";
        std::map<int,ArgValuePair>::const_iterator it = m_args.cbegin();
        for(; it != m_args.cend(); it++){
            const ArgValuePair& ap = it->second;
            os << "  " <<ap.arg.alias << "," << ap.arg.name << "\t\t" << ap.arg.comments << "\n";
        }
        if(!m_footer.empty()){
            os << m_footer << "\n";
        }
        return os.str();
    }

public:
    /**
     * @brief addArg
     * 不能ID重复，不能名字重复，不能name全是空
     * @param arg
     * @return
     */
    bool addOneArg(const Arg& arg,const ArgValue& value){
        if(contains(arg.getId())){
            return false;
        }
        if(!arg.alias.empty() && contains(arg.alias)){
            return false;
        }
        if(!arg.name.empty() && contains(arg.name)){
            return false;
        }
        ArgValuePair v(arg,value);
        m_args[arg.getId()] = v;
        return true;
    }

    int getNextArgId(){
        return m_args.size()+1;
    }

protected:
    /// 参数中是否有指定的名字了
    bool contains(const std::string& name_or_alias) const{
        Arg tmp(-1);
        return getArg(name_or_alias,tmp);
    }
    bool getArgValuePair(const std::string& name_or_alias,ArgValuePair& ap){
        std::map<int,ArgValuePair>::const_iterator it = m_args.cbegin();
        for(; it != m_args.cend() ; it++){
            if(it->second.arg.name == name_or_alias || it->second.arg.alias == name_or_alias){
                ap = it->second;
                return true;
            }
        }
        return false;
    }

    bool contains(const int& arg_id) const{
        std::map<int,ArgValuePair>::const_iterator it = m_args.cbegin();
        for(; it != m_args.cend() ; it++){
            if(it->second.arg.getId() == arg_id){
                return true;
            }
        }
        return false;
    }
protected:
    std::map<int,ArgValuePair> m_args;
    std::string m_header;
    std::string m_footer;
    std::string m_name_frefix;
    std::string m_alias_name_frefix;
    bool m_help;  /// 记录是否有输出help相关指令
};

/**
 * @brief The CmdArgHelper class
 * 控制台参数解析帮助类
 * 使用帮助：
 *
 */
class CmdArgHelper{
public:
    CmdArgHelper(){
    }

    /**
     * @brief parse
     * 解析参数，提取参数信息，解析完成之后可以通过isExist接口检测是否有指定的参数值。
     * @param argc
     * @param argv
     * @return
     */
    bool parse(int argc, char* argv[],std::string& error){
        if(argc <= 0){
            error = "参数个数小于0";
            return false;
        }
        args.clear();
        m_apppath = argv[0];
        m_appname = m_apppath.substr(m_apppath.find_last_of("/\\")+1);
        std::size_t found = m_apppath.find_last_of("/\\");
        if(found != std::string::npos){
            m_appdir = m_apppath.substr(0,found);
        }
        for(int i=1; i<argc; i++){
            args.push_back(argv[i]);
        }
        return true;
    }

    /**
     * @brief parse
     * 根据builder的定义解析参数，如果不符合规则变报错.
     * 解析完成之后可以通过isExist接口检测是否有指定的参数值。
     * 并且相关的参数值也将设置到Builder的引用中。
     * @param builder
     * @param argc
     * @param argv
     * @return
     */
    bool parse(CmdArgBuilder* builder,int argc, char* argv[],std::string& error){
        if(builder == NULL){
            error = "builder为空指针";
            return false;
        }
        if(!parse(argc,argv,error)){
            return false;
        }
        /// 根据builder定义解析参数
        return builder->parse(args,error);
    }

    /**
     * @brief isExist
     * 是否存在指定的参数，比如 "-d", "*p"
     * 比如test.exe -d -f D:\\t\tttc *p
     * 这里只是检测各个参数列表，是否有指定参数，并不做匹配等关联逻辑
     * @param arg
     * @return
     */
    bool isExist(const std::string& arg){
        for(int i=0,cnt=args.size(); i<cnt; i++){
            if(args[i] == arg){
                return true;
            }
        }
        return false;
    }

    /**
     * @brief isExist
     * 是否存在指定的参数，并把参数后面的紧跟值取出来
     * @param arg
     * @param value
     * @return
     */
    bool isExist(const std::string& arg,std::string& value){
        for(int i=0,cnt=args.size(); i<cnt-1; i++){
            if(args[i] == arg){
                value = args[i+1];
                return true;
            }
        }
        return false;
    }

    const std::string& getAppName() const {
        return m_appname;
    }

    const std::string& getAppPath() const {
        return m_apppath;
    }

    const std::string& getAppDir() const {
        return m_appdir;
    }

    /**
     * @brief getLaunchCmd
     * 获取启动的指令就是程序启动的命令行参数
     * @param include_path  是否包括启动路径
     * @return
     */
    std::string getLaunchCmd(bool include_path = false){
        std::ostringstream os;
        if(include_path){
            os << m_apppath;
        } else {
            os << m_appname;
        }
        for(int i=0,cnt=args.size(); i<cnt; i++){
            os << " ";
            os << args[i];
        }
        return os.str();
    }

private:
    /// APP的程序名字，比如main.exe
    std::string m_appname;
    /// APP的程序启动路径，比如C:\\testpath\test2\main.exe
    std::string m_apppath;
    /// APP的程序启动目录，比如C:\\testpath\test2
    std::string m_appdir;
    /// 参数列表，按顺序存放
    std::vector<std::string> args;

};

}
