/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2016-05-12
*
* @note
*******************************************************************************/

#pragma once
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <Utiles/StringProperties.hpp>

namespace utiles {

class ParamRepalce
{
private:
const char* PARAM_HEAD   = "${";
const char* PARAM_BODY   = "param";
const char* PARAM_TAIL   = "}";
private:
    bool start_with(const char* src,const char* test)
    {
        return (memcmp(src,test,strlen(test)) == 0);
    }

    /**
     * @brief 找尾部，如果中途遇到了头，就直接报错返回
     * @param src
     * @param start
     * @return
     */
    int find_tail_mark(const char* src,int start)
    {
        int len = strlen(src);
        int idx = 0;

        for(int i=start; i<len; i++)
        {
            // 还没找到尾，又遇到头了，那么简单判定为失败
            // 以后可能可以配对查找，嵌套。但是意义不大
            if( start_with(src+i,PARAM_HEAD))
            {
                idx = -1;
                break;
            }

            if( start_with(src+i,PARAM_TAIL))
            {
                idx = i;
                break;
            }
        }
        return idx;
    }

    /**
     * @brief 提取第一个param的内容,只在[start,end)区间提取param*的内容.
     *      param1就返回1
     * @param src
     * @param start
     * @param end
     * @return
     */
    int resolve_param_id(const char* src,int start,int end)
    {
        const int PARAM_LEN = strlen(PARAM_BODY);
        int idx = -1;

        for(int i=start; i<end; i++)
        {
            if( start_with(src+i,PARAM_BODY))
            {
                std::string tmp(src + i + PARAM_LEN, src + end);
                boost::trim_if(tmp,utiles::IsWhiteChar);
                try {
                    idx = boost::lexical_cast<int>(tmp);
                } catch (...) {
                }
            }
        }
        return idx;
    }

public:
    /**
     * @brief 解析tmplate，并用params填充模板
     * @param params    参数列表，第一个参数替换tmplate中的${param1},
     *                  第二个参数替换tmplate中的${param2},依次类推。
     *                  如果${param<i>}没有对应的参数可替换(参数个数少了),那么就会被替换为空
     *                  如果参数多了，那么内部会自动忽略。
     * @param tmplate   带有${param<i>}的字符串
     * @return 返回被替换之后的本文
     */
    std::string FillTemplate(const std::vector<std::string>& params, const std::string& tmplate)
    {
        const char*         src         = tmplate.c_str();
        int                 len         = tmplate.length();
        int                 idx         = 0;
        const int           HEAD_LEN    = strlen(PARAM_HEAD);
        std::stringstream   os;
        const int           PARAM_CNT   = params.size();

        // 参数格式要求 ${  paramN  }
        while(idx < len)
        {
            // 找头
            if( start_with(src+idx,PARAM_HEAD) )
            {
                // 找尾
                int tail = find_tail_mark(src,idx + HEAD_LEN);
                if(tail < 0)
                {
                    goto SAVE_GO_NEXT_POS;
                }
                BOOST_ASSERT(tail >= idx + HEAD_LEN);

                // 检测内容是不是param*的格式
                int index = resolve_param_id(src,idx + HEAD_LEN,tail);
                index --; // 转换为我们的索引
                if(index < 0)
                {
                    goto SAVE_GO_NEXT_POS;
                }

                // 参数替换
                // 如果超出了现有参数，那么用空串代替，也就是直接跳过即可
                if(index >= PARAM_CNT)
                {
                    idx = tail+1; // 跳过占位符
                    continue;
                }

                // 替换
                os << params[index];
                idx = tail+1; // 不再保存占位符
                continue;
            }

            SAVE_GO_NEXT_POS:
            os << src[idx];
            idx ++;
        }
        return os.str();
    }
};

}
