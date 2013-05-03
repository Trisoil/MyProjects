// DacInfo.h

#ifndef _PPBOX_DAC_DAC_INFO_H_
#define _PPBOX_DAC_DAC_INFO_H_

#include "ppbox/dac/DacType.h"

#include <util/archive/TextOArchive.h>

#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/base_from_member.hpp>

namespace ppbox
{
    namespace dac
    {

        struct dac_time
        {
            boost::uint32_t time;

            dac_time(
                boost::uint32_t t = (boost::uint32_t)-1)
            {
                time = t;
            }

            dac_time & operator=(
                boost::uint32_t t)
            {
                time = t;
                return *this;
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & time;
            }
        };

        struct dac_errno
            : boost::system::error_code
        {
            dac_errno(
                boost::system::error_code const & ec = boost::system::error_code())
                : boost::system::error_code(ec)
            {
            }

            dac_errno & operator=(
                boost::system::error_code const & ec)
            {
                (boost::system::error_code &)(*this) = ec;
                return *this;
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                if (category() == boost::system::get_system_category()) {
                    boost::system::errc::errc_t const * iter = std::find(ErrorTypes, 
                        ErrorTypes + sizeof(ErrorTypes) / sizeof(ErrorTypes[0]), 
                        category().default_error_condition(value()).value());
                    if (iter != ErrorTypes + sizeof(ErrorTypes) / sizeof(ErrorTypes[0])) {
                        ar & std::distance(ErrorTypes, iter);
                        return;
                    }
                }
                ar & value();
            }
        };

        class DacArchive
            : private boost::base_from_member<std::ostringstream>
            , public util::archive::TextOArchive<>
        {
        public:
            DacArchive()
                : util::archive::TextOArchive<>(member)
            {
                set_delim("=");
                set_space("&");
            }

        public:
            std::string const str() const
            {
                return member.str();
            }
        };

        struct DacBaseInfo        // 通用的基本信息
        {
            // 接口类别
            InterfaceType::Enum interface_type;

            // 接口版本
            boost::uint16_t interface_ver;

            // 客户信息
            std::string gid;

            // 项目渠道信息
            std::string pid;

            // 用户的BOXID
            std::string bid;

            // 用户提交数据的时间
            time_t sub_time;

            // SDK版本号
            std::string ppbox_ver;

            // SDK类别(10-STB中间层,11-STB适配层)
            StbType::Enum stb_type;

            DacBaseInfo(
                InterfaceType::Enum e)
                : interface_type(e)
            {
                sub_time = time(NULL);
            }

            virtual ~DacBaseInfo() {}

            virtual bool has_core_info() { return false; }

            virtual bool ignore_this() { return false; }

            virtual LogReason::Enum need_log(
                boost::system::error_code & ec)
            {
                return LogReason::none;
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                size_t Action = 0;
                ar & SERIALIZATION_NVP(Action);
                ar & SERIALIZATION_NVP_NAME("A", interface_type);
                ar & SERIALIZATION_NVP_NAME("B", interface_ver);
                ar & SERIALIZATION_NVP_NAME("C", gid);
                ar & SERIALIZATION_NVP_NAME("D", pid);
                ar & SERIALIZATION_NVP_NAME("E", bid);
                ar & SERIALIZATION_NVP_NAME("F", sub_time);
                ar & SERIALIZATION_NVP_NAME("G", ppbox_ver);
                ar & SERIALIZATION_NVP_NAME("H", stb_type);
            }
        };

        // 内核通用的信息
        struct DacCoreCommInfo
            : DacBaseInfo
        {
            // 当前状态：空闲还是点播还是直播：0-表示空闲,1-表示点播,2-表示直播
            int core_type;

            // 内核版本号
            std::string core_ver;

            DacCoreCommInfo(
                InterfaceType::Enum e)
                : DacBaseInfo(e)
                , core_type(0)
            {
            }

            virtual bool has_core_info() { return true; }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacBaseInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("I", core_type);
                ar & SERIALIZATION_NVP_NAME("J", core_ver);
            }
        };

        template <
            typename Base, 
            typename Info
        >
        struct DacOnlyOnce
            : Base
        {
            DacOnlyOnce(
                InterfaceType::Enum e)
                : Base(e)
            {
            }

            static bool already()
            {
                static bool sb = false;
                bool b = sb;
                sb = true;
                return b;
            };

            virtual bool ignore_this() { return already(); }
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_H_
