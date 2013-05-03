// IPpbox.cpp

#include "ppbox/ppbox/Common.h"

#include <ppbox/certify/CertifyError.h>
#include <ppbox/demux/base/DemuxError.h>

#include <util/protocol/http/HttpError.h>

#include <boost/asio/error.hpp>

namespace ppbox
{

    namespace error
    {

        error::errors last_error_enum(
            boost::system::error_code const & ec)
        {
            if (!ec) {
                return success;
            } else if (ec == boost::asio::error::would_block) {
                return would_block;
            } else if (ec == ppbox::demux::error::no_more_sample) {
                return stream_end;
            } else if (ec == boost::asio::error::operation_aborted) {
                return operation_canceled;
            } else if (ec == boost::asio::error::connection_aborted) {
                return operation_canceled;
            } else if (ec.category() == ppbox::error::get_category()) {
                return (error::errors)ec.value();
            } else if (ec.category() == ppbox::certify::error::get_category()) {
                return certify_error;
#ifdef PPBOX_CERTIFY_ERROR_AUTH_ERRORS
            } else if (ec.category() == ppbox::certify::error::get_auth_category()) {
                return certify_error;
#endif
#ifdef PPBOX_CERTIFY_ERROR_MUTEX_ERRORS
            } else if (ec.category() == ppbox::certify::error::get_mutex_category()) {
                return certify_error;
#endif
            } else if (ec.category() == ppbox::demux::error::get_category()) {
                return demux_error;
            } else if (ec.category() == framework::system::logic_error::get_category()) {
                return ppbox::error::logic_error;
            } else if (ec.category() == util::protocol::http_error::get_category()) {
                return network_error;
            } else if (ec.category() == boost::asio::error::get_system_category()) {
                return network_error;
            } else if (ec.category() == boost::asio::error::get_netdb_category()) {
                return network_error;
            } else if (ec.category() == boost::asio::error::get_addrinfo_category()) {
                return network_error;
            } else if (ec.category() == boost::asio::error::get_misc_category()) {
                return network_error;
            } else {
                return other_error;
            }
        }

    } // namespace error

} // namespace ppbox
