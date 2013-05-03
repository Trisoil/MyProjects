// Error.h

#ifndef _PPBOX_AVFORMAT_ERROR_H_
#define _PPBOX_AVFORMAT_ERROR_H_

namespace ppbox
{
    namespace avformat
    {

        namespace error {

            enum errors
            {
                avformat_success =0,
                invalid_mp4_head , 
                invalid_mp4_truck,
            };

            namespace detail {

                class live_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "avformat";
                    }

                    std::string message(int value) const
                    {
                        switch (value) 
                        {
                        case avformat_success:
                            return "operator success";
                            case invalid_mp4_head:
                                return "invalid mp4 head";
                            case invalid_mp4_truck:
                                return "invalid mp4 truck";
                            default:
                                return "avformat other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::live_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace live_error

    } // namespace live
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::avformat::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::avformat::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_PPAP_ERROR_H_
