#ifndef _PTELNET_CODECONVERT_H_
#define _PTELNET_CODECONVERT_H_

#include <string>
#include <boost/system/error_code.hpp>

namespace ptelnet
{
    class CodeConvert
    {
    public:
        // ����һ��ת������ָ��Դ�����ʽ��Ŀ������ʽ
        CodeConvert(
            std::string const & to_code, 
            std::string const & from_code);

        ~CodeConvert();


    public:
        // ʹ��ת����ת���ַ�����
        void convert(
            std::string const & from, 
            std::string & to);

        std::size_t err_pos() const
        {
            return err_pos_;
        }

    private:
        std::string t1; // Ŀ������ʽ
        std::string t2; // Դ�����ʽ
        std::size_t err_pos_;
    };
}

#endif