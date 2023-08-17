#pragma once

#include <QString>

class HrResultType
{
public:
    HrResultType()
    {
        m_rt = true;
        m_excptcode = 0;
    }

    HrResultType(bool rt)
    {
        m_rt = rt;
        m_excptcode = rt?0:-1;
    }

    HrResultType(bool rt, int code )
    {
        m_rt = rt;
        m_excptcode = code;
    }

    HrResultType(bool rt, const QString& describe )
    {
        m_rt = rt;
        m_excptdescribe = describe;
        m_excptcode = rt?0:-1;
    }
    HrResultType(bool rt, const std::string& describe )
    {
        m_rt = rt;
        m_excptdescribe = QString::fromStdString(describe);
        m_excptcode = rt?0:-1;
    }

    HrResultType(bool rt, int code, const QString& describe)
    {
        m_rt = rt;
        m_excptcode = code;
        m_excptdescribe = describe;
    }
    HrResultType(bool rt, int code, const std::string& describe)
    {
        m_rt = rt;
        m_excptcode = code;
        m_excptdescribe = QString::fromStdString(describe);
    }

    ~HrResultType()
    {

    }

    void InsertDescribe(const QString& describe,const QString& gap = ",")
    {
        if(!m_excptdescribe.isEmpty()){
            m_excptdescribe = describe + gap +m_excptdescribe;
        } else {
            m_excptdescribe = describe + m_excptdescribe;
        }
    }

    void AddDescribe(const QString& describe,const QString& gap = ",")
    {
        if(!m_excptdescribe.isEmpty()){
            m_excptdescribe += gap;
        }
        m_excptdescribe += describe;
    }

    void SetErrCode(int code)
    {
        m_excptcode = code;
    }
    void Set(bool rt, int code, const QString& describe)
    {
        m_rt = rt;
        m_excptcode = code;
        m_excptdescribe = describe;
    }
public:
    operator bool(void)
    {
        return m_rt;
    }

    operator int(void)
    {
        return m_excptcode;
    }

    operator QString(void)
    {
        return m_excptdescribe;
    }

    void operator +=(const QString& describe)
    {
        m_excptdescribe = describe + m_excptdescribe;
    }
    void operator()(bool rt, int code, const QString& describe)
    {
        m_rt = rt;
        m_excptcode = code;
        m_excptdescribe = describe;
    }

public:
    bool        m_rt;
    int         m_excptcode;
    QString     m_excptdescribe;
};
