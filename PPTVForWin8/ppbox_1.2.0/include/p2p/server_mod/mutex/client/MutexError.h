// MutexError.h

#ifndef _MUTEX_CLIENT_MUTEX_ERROR_H_
#define _MUTEX_CLIENT_MUTEX_ERROR_H_

namespace mutex
{
    namespace client
    {

        namespace error {

            enum errors
            {
                no_mutex_server = 1,
                use_all_mutex_server,
                time_out,
                other_error,

                response_error_delta = 100, 
                kickout, 
                mev_failed, 
            };

            namespace detail {

                class mutex_category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "mutex";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case no_mutex_server:
                                return "mutex: no server";
                            case use_all_mutex_server:
                                return "mutex: try all server";
                            case time_out:
                                return "mutex: time out";
                            case response_error_delta:
                                return "mutex: no error";
                            case kickout:
                                return "mutex: kickout";
                            case mev_failed:
                                return "mutex: mev check failed";
                            default:
                                return "mutex: other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::mutex_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

            static boost::system::error_category const & mutex_category = get_category();

        } // namespace error

    } // namespace client
} // namespace mutex

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum< ::mutex::client::error::errors >
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ::mutex::client::error::make_error_code;
#endif

    }
}

#endif // _MUTEX_CLIENT_MUTEX_ERROR_H_
