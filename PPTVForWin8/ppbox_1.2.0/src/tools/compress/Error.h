// Error.h

#ifndef _PPBOX_COMPRESS_ERROR_H_
#define _PPBOX_COMPRESS_ERROR_H_

namespace ppbox
{
    namespace compress
    {

        namespace error {

            enum errors
            {
                compress_success = 0,
                low_buffer,
                file_not_found,
                file_open_failed,
            };

            namespace detail {

                class compress_category
                    : public boost::system::error_category
                {
                public:
                    compress_category()
                    {
                        register_category(*this);
                    }

                    const char* name() const
                    {
                        return "compress";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case low_buffer:
                                return "low buffer size";
                            case file_not_found:
                                return "file not found";
                            case file_open_failed:
                                return "file open failed";
                            default:
                                return "compress other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::compress_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace error
    } // namespace compress
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::compress::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::compress::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_COMPRESS_ERROR_H_
