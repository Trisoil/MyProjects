// MediaBase.h

#ifndef _PPBOX_DATA_BASE_MEDIA_BASE_H_
#define _PPBOX_DATA_BASE_MEDIA_BASE_H_

#include "ppbox/data/base/MediaInfo.h"

#include <ppbox/common/ClassFactory.h>

namespace ppbox 
{
    namespace data 
    {

        class MediaBase
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            MediaBase(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~MediaBase();

        public:
            static MediaBase * create(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

        public:
            virtual void async_open(
                response_type const & resp) = 0;

            virtual void cancel(
                boost::system::error_code & ec) = 0;

            virtual void close(
                boost::system::error_code & ec)= 0;

        public:
            std::string const & get_protocol() const
            {
                return url_.protocol();
            }

            virtual bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const = 0;

            virtual bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const = 0;

            virtual bool get_url(
                framework::string::Url & url,
                boost::system::error_code & ec) const;

        public:
            virtual void on_error(
                boost::system::error_code & ec) const { /* do nothing */ };

        public:
            boost::asio::io_service & get_io_service()
            {
                return io_svc_;
            }

        private:
            boost::asio::io_service & io_svc_;

        protected:
            framework::string::Url url_;
        };

        class MediaProtocolFactory
            : public ppbox::common::ClassFactory<
                MediaBase, 
                std::string, 
                MediaBase * (
                    boost::asio::io_service &,
                    framework::string::Url const &)
                >
        {
        };

        class MediaFormatFactory
            : public ppbox::common::ClassFactory<
                MediaBase, 
                std::string, 
                MediaBase * (
                    boost::asio::io_service &,
                    framework::string::Url const &)
            >
        {
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_MEDIA_BY_PROTOCOL(k, c) PPBOX_REGISTER_CLASS_FACTORY(k, ppbox::data::MediaProtocolFactory, c)
#define PPBOX_REGISTER_MEDIA_BY_FORMAT(k, c) PPBOX_REGISTER_CLASS_FACTORY(k, ppbox::data::MediaFormatFactory, c)

#endif//_PPBOX_DATA_BASE_MEDIA_BASE_H_
