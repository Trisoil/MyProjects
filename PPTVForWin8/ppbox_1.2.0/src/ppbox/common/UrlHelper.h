// UrlHelper.h

#ifndef _PPBOX_COMMON_URL_HELPER_H_
#define _PPBOX_COMMON_URL_HELPER_H_

#include <framework/string/Url.h>
#include <framework/configure/Config.h>

namespace ppbox
{
    namespace common
    {

            bool decode_url(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void apply_config(
                framework::configure::Config & config, 
                framework::string::Url const & url, 
                std::string const & prefix);

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_URL_HELPER_H_
