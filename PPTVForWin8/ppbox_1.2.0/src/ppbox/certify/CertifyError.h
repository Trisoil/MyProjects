// CertifyError.h

#ifndef _PPBOX_CERTIFY_CERTIFY_ERROR_H_
#define _PPBOX_CERTIFY_CERTIFY_ERROR_H_

#include <mutex/client/MutexError.h>

namespace ppbox
{
    namespace certify
    {

        namespace error {

            enum errors
            {
                not_started = 1, 
                alive_dead, 
                no_vod_key, 
                no_live_key
            };

#define PPBOX_CERTIFY_ERROR_AUTH_ERRORS

            enum auth_errors
            {
                wrong_response_packet = 1, 
                wrong_response_key, 
                time_out, 
                response_error,

                response_error_delta = 100, 
                no_such_project, 
                too_more_device, 
                duplicate_device, 
                wrong_request_packet, 
                white_denied,
                black_denied,
                version_denied,
            };

#define PPBOX_CERTIFY_ERROR_MUTEX_ERRORS

            typedef mutex::client::error::errors mutex_errors;

            namespace detail {

                class certify_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "certify";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case not_started:
                                return "certify: not started";
                            case alive_dead:
                                return "certify: alive dead";
                            case no_vod_key:
                                return "certify: no vod key";
                            case no_live_key:
                                return "certify: no live key";
                            default:
                                return "certify: unknown error";
                        }
                    }
                };

                class certify_auth_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "certify.auth";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case wrong_response_packet:
                                return "auth: response packet error";
                            case wrong_response_key:
                                return "auth: response key error";
                            case time_out:
                                return "auth: time out";
                            case response_error:
                                return "auth: response error";

                            case no_such_project:
                                return "auth: no such project";
                            case too_more_device:
                                return "auth: too more devices in this project";
                            case duplicate_device:
                                return "auth: duplicate devices detect";
                            case wrong_request_packet:
                                return "auth: request packet error";
                            case white_denied:
                                return "auth: white denied";
                            case black_denied:
                                return "auth: black denied";
                            case version_denied:
                                return "auth: version denied";
                            default:
                                return "auth: other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::certify_category instance;
                return instance;
            }

            inline const boost::system::error_category & get_auth_category()
            {
                static detail::certify_auth_category instance;
                return instance;
            }

            inline const boost::system::error_category & get_mutex_category()
            {
                return mutex::client::error::get_category();
            }

            static boost::system::error_category const & certify_category = get_category();

            static boost::system::error_category const & certify_auth_category = get_auth_category();

            static boost::system::error_category const & certify_mutex_category = get_mutex_category();

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

            inline boost::system::error_code make_error_code(
                auth_errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_auth_category());
            }

        } // namespace certify_error

    } // namespace certify
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::certify::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

        template<>
        struct is_error_code_enum<ppbox::certify::error::auth_errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::certify::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_CERTIFY_CERTIFY_ERROR_H_
