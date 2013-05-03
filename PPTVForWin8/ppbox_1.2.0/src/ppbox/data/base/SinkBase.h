// SinkBase.h

#ifndef _PPBOX_DATA_BASE_SINK_BASE_H_
#define _PPBOX_DATA_BASE_SINK_BASE_H_

#include "ppbox/data/base/DataBase.h"

#include <ppbox/common/ClassFactory.h>

#include <util/stream/Sink.h>

#include <framework/string/Url.h>

namespace ppbox
{
    namespace data
    {

        class SinkBase
            : public util::stream::Sink
            , public ppbox::common::ClassFactory<
                SinkBase, 
                std::string, 
                SinkBase * (boost::asio::io_service &)
            >
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

        public:
            static SinkBase * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

        public:
            SinkBase(
                boost::asio::io_service & io_svc);

            virtual ~SinkBase();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::system::error_code & ec) = 0;

            virtual void async_open(
                framework::string::Url const & url,
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec) = 0;

            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec) = 0;

        public:
            virtual bool continuable(
                boost::system::error_code const & ec);

            virtual bool recoverable(
                boost::system::error_code const & ec);
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_SINK(k, c) PPBOX_REGISTER_CLASS(k, c)

#endif // _PPBOX_DATA_BASE_SINK_BASE_H_
