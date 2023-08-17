/*******************************************************************************
* @version   V1.0.0
* @author    LiFeng,
* @date      2017-04-11
*
* @note
*******************************************************************************/

#pragma once
#include <functional>

namespace utiles {

/**
 * @brief 一个实现构造时调用指定函数，析构时调用指定函数的类。
 *  用于内存守护，变量的临时标记与恢复等等。
 */
class AutoCall
{
public:
    typedef std::function<void()> Fun;
public:
    /**
     * @brief AutoCall          构造一个自动调用的类。该类构造时会调用construct_call函数，
     *                          析构时会调用destruction_call函数
     * @param construct_call
     * @param destruction_call
     */
    AutoCall(Fun construct_call,Fun destruction_call)
    {
        m_dtc = destruction_call;
        called_end = false;
        construct_call();
    }

    /**
     * @brief AutoCall          构造一个自动调用的类.析构时会调用destruction_call函数
     * @param destruction_call
     */
    AutoCall(Fun destruction_call)
    {
        m_dtc = destruction_call;
        called_end = false;
    }

    void End()
    {
        called_end = true;
        m_dtc();
    }

    /**
     * @brief 不做任何操作
     */
    void MakeCompilerHappy()
    {
    }

    virtual ~AutoCall()
    {
        if(!called_end)
        {
            End();
        }
    }

public:
    /**
     * @brief 调用delete，给外界提供自动释放内存的功能
     * @param ptr
     */
    static void DeleteFun(void* ptr)
    {
        if(ptr)
            delete ptr;
    }
    /**
     * @brief 调用delete []，给外界提供自动释放内存的功能
     * @param ptr
     */
    static void DeleteArrayFun(void* ptr)
    {
        if(ptr)
            delete [] ptr;
    }

    /**
     * @brief 使用free释放函数
     * @param ptr
     */
    static void FreeFun(void* ptr)
    {
        if(ptr)
            free(ptr);
    }

protected:
    Fun m_dtc;
    bool called_end;
};

}
