// CommonError.h

#ifndef _PPBOX_COMMON_ERROR_H_
#define _PPBOX_COMMON_ERROR_H_

namespace ppbox
{
    namespace common
    {
        namespace error {
            enum errors
            {
                common_success = 0,
                not_support,
                not_find_session,
                wrong_status,
                canceled_moive,
                play_not_open_moive,
            };

            namespace detail {

                class mux_category
                    : public boost::system::error_category
                {
                public:
                    mux_category()
                    {
                        register_category(*this);
                    }

                    const char* name() const
                    {
                        return "common";
                    }

                    std::string message(int value) const
                    {
                        switch (value) {
                            case common_success:
                                return "sucess";
                            case not_support:
                                return "common not support";
                            case not_find_session:
                                return "common: not find session";
                            case wrong_status:
                                return "common: wrong go to here";
                            case canceled_moive:
                                return "common: canceled moive";
                            case play_not_open_moive:
                                return "common: play not open moive";
                            default:
                                return "common other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::mux_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace error

    } // namespace common
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::common::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::common::error::make_error_code;
#endif

    }
}

#endif // _PPBOX_COMMON_ERROR_H_
