// SourceError.h

#ifndef _PPBOX_DATA_BASE_SOURCE_ERROR_H_
#define _PPBOX_DATA_BASE_SOURCE_ERROR_H_

namespace ppbox
{
    namespace data
    {

        namespace source_error {

            enum errors
            {
                no_more_segment = 1, 
                at_end_point = 2, 
                need_reopen,
            };

            namespace detail {
                class source_category
                    : public boost::system::error_category
                {
                public:
                    source_category()
                    {
                        register_category(*this);
                    }

                    const char* name() const
                    {
                        return "data_source";
                    }

                    std::string message(int value) const
                    {
                        if (value == source_error::no_more_segment)
                            return "data source: has no more segments";
                        if (value == source_error::at_end_point)
                            return "data source: at end point, write complete";
                        if (value == source_error::need_reopen)
                            return "data source: need reopen";
                        return "data source: unknown error";
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::source_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace source_error

    } // namespace data
} // namespace ppbox

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<ppbox::data::source_error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using ppbox::data::source_error::make_error_code;
#endif

    }
}

#endif // _PPBOX_DATA_BASE_SOURCE_ERROR_H_
