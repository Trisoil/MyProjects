#ifndef _PTELNET_CODECONVERT_H_
#define _PTELNET_CODECONVERT_H_

#include <string>
#include <boost/system/error_code.hpp>

namespace ptelnet
{
    class CodeConvert
    {
    public:
        // 构造一定转换器，指定源编码格式和目标编码格式
        CodeConvert(
            std::string const & to_code, 
            std::string const & from_code);

        ~CodeConvert();


    public:
        // 使用转换器转换字符编码
        void convert(
            std::string const & from, 
            std::string & to);

        std::size_t err_pos() const
        {
            return err_pos_;
        }

    private:
        std::string t1; // 目标编码格式
        std::string t2; // 源编码格式
        std::size_t err_pos_;
    };
}

#endif