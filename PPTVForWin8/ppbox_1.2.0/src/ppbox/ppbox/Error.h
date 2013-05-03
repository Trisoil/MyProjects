// Error.h

#ifndef _PPBOX_PPBOX_ERROR_H_
#define _PPBOX_PPBOX_ERROR_H_

namespace ppbox
{

    namespace error {

        enum errors
        {
            success = 0, 
            not_start, 
            already_start, 
            not_open, 
            already_open, 
            operation_canceled, 
            would_block, 
            stream_end, 
            logic_error, 
            network_error, 
            demux_error, 
            certify_error, 
            download_error,
            other_error = 1024, 
        };

        namespace detail {

            class category
                : public boost::system::error_category
            {
            public:
                const char* name() const
                {
                    return "ppbox/ppbox";
                }

                std::string message(int value) const
                {
                    if (value == success)
                        return "Everything is ok";
                    if (value == not_start)
                        return "PPBOX has not started";
                    if (value == already_start)
                        return "PPBOX has already started";
                    if (value == not_open)
                        return "PPBOX has not opened";
                    if (value == already_open)
                        return "PPBOX has already opened";
                    if (value == operation_canceled)
                        return "PPBOX operation canceled";
                    if (value == would_block)
                        return "PPBOX stream would block";
                    if (value == stream_end)
                        return "PPBOX stream end";
                    if (value == logic_error)
                        return "PPBOX logic error";
                    if (value == network_error)
                        return "PPBOX network error";
                    if (value == demux_error)
                        return "PPBOX demux error";
                    if (value == certify_error)
                        return "PPBOX certify error";
                    if (value == download_error)
                        return "PPBOX download error";
                    return "PPBOX error";
                }
            };

        } // namespace detail

        inline const boost::system::error_category & get_category()
        {
            static detail::category instance;
            return instance;
        }

        static boost::system::error_category const & ppbox_category = get_category();

        inline boost::system::error_code make_error_code(
            errors e)
        {
            return boost::system::error_code(
                static_cast<int>(e), get_category());
        }

        inline boost::system::error_code & __last_error()
        {
            static boost::system::error_code ec;
            return ec;
        }

        inline boost::system::error_code const & last_error()
        {
            return __last_error();
        }

        inline void last_error(
            boost::system::error_code const & ec)
        {
            __last_error() = ec;
        }

        error::errors last_error_enum(
            boost::system::error_code const & ec);

        inline error::errors last_error_enum()
        {
            return last_error_enum(last_error());
        }

    } // namespace error

} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_PPBOX_ERROR_H_
