// DacInfoLog.h

#ifndef _PPBOX_DAC_DAC_INFO_LOG_H_
#define _PPBOX_DAC_DAC_INFO_LOG_H_

#include "ppbox/dac/DacType.h"

namespace ppbox
{
    namespace dac
    {

        struct DacLogInfo
        {
            // �ͻ���Ϣ
            std::string gid;

            // ��Ŀ������Ϣ
            std::string pid;

            // SDK�汾��
            std::string version;

            // �û���BOXID
            std::string mac;

            // �û���BOXID
            LogReason::Enum reason;

            dac_errno ec;

            DacLogInfo(
                LogReason::Enum reason, 
                boost::system::error_code const & ec)
                : reason(reason)
                , ec(ec)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & SERIALIZATION_NVP(gid);
                ar & SERIALIZATION_NVP(pid);
                ar & SERIALIZATION_NVP(version);
                ar & SERIALIZATION_NVP(mac);
                ar & SERIALIZATION_NVP(reason);
                ar & SERIALIZATION_NVP(ec);
            }
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_LOG_H_
