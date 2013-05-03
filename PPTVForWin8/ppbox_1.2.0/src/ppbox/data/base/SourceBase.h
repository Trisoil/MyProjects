// SourceBase.h

#ifndef _PPBOX_DATA_BASE_SOURCE_BASE_H_
#define _PPBOX_DATA_BASE_SOURCE_BASE_H_

#include "ppbox/data/base/DataBase.h"

#include <ppbox/common/ClassFactory.h>

#include <util/stream/Source.h>

namespace ppbox
{
    namespace data
    {

        class SourceBase
            : public util::stream::Source
            , public ppbox::common::ClassFactory<
                SourceBase, 
                std::string, 
                SourceBase * (boost::asio::io_service &)
            >
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

        public:
            static SourceBase * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

            static SourceBase * create(
                boost::asio::io_service & io_svc,
                MediaBase & media);

        public:
            SourceBase(
                boost::asio::io_service & io_svc);

            virtual ~SourceBase();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec) = 0;

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec)= 0;

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

            virtual bool recoverable(
                boost::system::error_code const & ec);
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_SOURCE(k, c) PPBOX_REGISTER_CLASS(k, c)

#endif // _PPBOX_DATA_BASE_SOURCE_BASE_H_
