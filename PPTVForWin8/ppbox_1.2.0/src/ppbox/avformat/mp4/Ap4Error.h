// Error.h

#ifndef _PPBOX_AVFORMAT_ERROR_H_
#define _PPBOX_AVFORMAT_ERROR_H_

namespace ppbox
{
    namespace avformat
    {

        namespace ap4_error {

            enum errors
            {
                invalid_mp4_head = 1, 
                invalid_mp4_truck,
            };

            namespace detail {

                class ap4_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "ap4";
                    }

                    std::string message(int value) const
                    {
                        switch (value) 
                        {
                            case invalid_mp4_head:
                                return "invalid mp4 head";
                            case invalid_mp4_truck:
                                return "invalid mp4 truck";
                            default:
                                return "ap4 other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::ap4_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace ap4_error

    } // namespace live
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::avformat::ap4_error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::avformat::ap4_error::make_error_code;
#endif

    }
}

#endif // _PPBOX_PPAP_ERROR_H_
