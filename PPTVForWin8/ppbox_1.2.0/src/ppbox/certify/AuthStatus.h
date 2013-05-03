// AuthStatus.h

#ifndef _PPBOX_CERTIFY_AUTH_STATUS_H_
#define _PPBOX_CERTIFY_AUTH_STATUS_H_

namespace ppbox
{
    namespace certify
    {

        struct AuthStatus
        {
            enum Enum
            {
                stopped, // 没有调用startup
                authing, 
                authed, 
                failed, 
                time_out, 
            };

            static inline std::string status_str(
                Enum status)
            {
                switch (status) {
                    case stopped:
                        return "stopped";
                    case authing:
                        return "authing";
                    case authed:
                        return "authed";
                    case failed:
                        return "failed";
                    case time_out:
                        return "time_out";
                    default:
                        return "unknown status";
                }
            }
        };

    } // namespace certify
} // namespace ppbox

#endif // _PPBOX_CERTIFY_AUTH_STATUS_H_

