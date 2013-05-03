// Auth.cpp

#include "ppbox/alive/Common.h"
#include "ppbox/alive/Auth.h"

#include <ppbox/common/DomainName.h>
#include <ppbox/certify/CertifyError.h>

using namespace ppbox::certify;
using namespace ppbox::certify::error;
using namespace mutex::client;

#include <util/protocol/pptv/Url.h>
using namespace util::protocol;
using namespace util::protocol::pptv;
using namespace util::protocol::http_error;

#include <framework/logger/StreamRecord.h>
#include <framework/string/Format.h>
#include <framework/string/Parse.h>
#include <framework/string/Base16.h>
#include <framework/string/Md5.h>
using namespace framework::string;
using namespace framework::network;

#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::error;

#include <tinyxml/tinyxml.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.alive.Auth", framework::logger::Debug);


#ifndef PPBOX_DNS_AUTH
#define PPBOX_DNS_AUTH "(tcp)(v4)cert.150hi.com:8008"
#endif

namespace ppbox
{
    namespace alive
    {
//#ifdef API_PPLIVE
//        static const NetName dns_auth_server("(tcp)(v4)c.api.pplive.com:8008");
//#else
        DEFINE_DOMAIN_NAME(dns_auth_server,PPBOX_DNS_AUTH);
//#endif

        static const char PPBOX_KEY_REQ[] = "jsd0af9(%31";
        static const char PPBOX_KEY_RES[] = "0a0w495(_E";
        static const char PPBOX_KEY[] = "kioe257ds";

        Auth::Auth(
            boost::asio::io_service & io_svc)
            : random_(0) 
            , http_(io_svc)
            , auth_retry_times_(0)
            , timer_(new clock_timer(io_svc))
        {
            srand((boost::uint32_t)time(NULL));
        }

        Auth::~Auth()
        {
            error_code ec;
            if (timer_) {
                delete timer_;
                timer_ = NULL;
            }
        }

        void Auth::async_certify(
            AuthRequest const & request,
            certify_response_type const & resp)
        {
            certify_response_ = resp;
            HttpRequestHead head;
            head.method = HttpRequestHead::get;
            head.host = dns_auth_server.host() + ":" + dns_auth_server.svc();
            random_ = rand();

            head.path = "/ppbox-auth/auth2?" + get_auth_string(request);
            auth_retry_times_ = 1;
            http_.async_fetch(head,
                boost::bind(&Auth::handle_certify, this, _1));
        }

        void Auth::cancel(
            boost::system::error_code & ec)
        {
            http_.cancel(ec);
            timer_->cancel(ec);
        }

        void Auth::handle_certify(
            error_code const & ec)
        {
            LOG_DEBUG("[handle_certify] ec: " << ec.message());

            if (auth_retry_times_ == 1) {
                first_stat_ = http_.stat();
            }

            boost::system::error_code ec1 = ec;

            if (HttpClient::recoverable(ec1)) {
                boost::uint32_t wait_times = 2 << auth_retry_times_;
                ++auth_retry_times_;
                if (wait_times < 30) {
                    timer_->expires_from_now(Duration::seconds(wait_times));
                    timer_->async_wait(boost::bind(&Auth::on_certify_timer, this, ec));
                    return;
                } else {
                    ec1 = ppbox::certify::error::time_out;

                    LOG_DEBUG(" timeout: " << ec1.message());
                }
            } else {
                error_code ec2;
                http_.close(ec2);
            }

            AuthResponse authrespone;
            if (!ec1) {
                parse_response(authrespone, ec1);
            }

            if (auth_retry_times_ == 1) {
                first_stat_.last_error = ec1;
            }

            certify_response_type resp;
            resp.swap(certify_response_);
            resp(authrespone, ec1);
        }

        void Auth::on_certify_timer(
            error_code const & ec)
        {
            if (ec)
                return;

            http_.async_fetch(http_.request_head(), 
                boost::bind(&Auth::handle_certify, this, _1));
        }

        std::string Auth::get_auth_string(
            AuthRequest const & request) const
        {
            std::string key;
            key += "HID=" + request.hid;
            key += "&MID=" + request.mid;
            key += "&AID=" + request.aid;
            key += "&SV=" + request.version;
            key += "&KV=0.0.0.0";
            key += "&gid=" + request.gid;
            key += "&pid=" + request.pid;
            key += "&auth=" + request.auth;
            key += "&random=" + format(random_);
            return url_encode(key, PPBOX_KEY_REQ);
        }

        error_code Auth::decrypt_md5(
            char const * key,
            error_code & ec)
        {
            ec = error_code();
            if (key == NULL) {
                ec = wrong_response_key;
            } else {  
                Md5 md5;
                std::string key_md5(key);
                std::string key_md5_check;
                key_md5_check += PPBOX_KEY;
                key_md5_check += format(random_);
                md5.update((unsigned char *)&key_md5_check[0], key_md5_check.size());
                md5.final();
                std::string digest((char const *)md5.digest(), 16);
                if (Base16::encode(digest, false) != key)
                    ec = wrong_response_key;
            }
            return ec;
        }

        error_code Auth::parse_response(
            AuthResponse & authrespone,
            error_code & ec)
        {
            ec = error_code();
            authrespone.init();
            TiXmlDocument xmlDoc;
            *boost::asio::buffer_cast<char *>(http_.response().data().prepare(1)) = '\0';
            http_.response().data().commit(1);
#ifdef _TEST_
            std::string buffer = boost::asio::buffer_cast<char const *>(http_.get_response().data().data());
#else
            std::string buffer = url_decode(boost::asio::buffer_cast<char const *>(http_.response().data().data()), PPBOX_KEY_RES);
#endif
            xmlDoc.Parse(buffer.c_str());
            if (xmlDoc.Error()) {
                return ec = wrong_response_packet;
            }
            
            TiXmlElement * xml_result = xmlDoc.RootElement();
            if (!xml_result) {
                return ec = wrong_response_packet;
            }
            char const * pText = NULL;
            TiXmlElement * element = NULL;
            if (!ec) {
                element = xml_result->FirstChildElement("succeed");
                if (element && (pText = element->GetText())) {
                    authrespone.isSuccess = parse<bool>(pText);//(pText[0] == '1' && pText[1] == '\0');
                } else {
                    ec = wrong_response_packet;
                }
            }
            if (!ec) {
                element = xml_result->FirstChildElement("key");
                if (element && (pText = element->GetText())) {
                    authrespone.key = pText;

                    //TODO:
                    //if (!ec) {
                    //    ec = decrypt_md5(authrespone.key, ec);
                    //}
                } else {
                    ec = wrong_response_packet;
                }
            }
            if (!ec && authrespone.isSuccess) {
                element = xml_result->FirstChildElement("session_id");
                if (element && (pText = element->GetText())) {
                    authrespone.session_id = pText;
                } else {
                    ec = wrong_response_packet;
                }
                if (!ec) {
                    element = xml_result->FirstChildElement("vod_playlink_key");
                    if (element && (pText = element->GetText())) {
                        authrespone.vod_playlink_key = pText;
                    } else {
                        //ec = wrong_response_packet;
                    }
                }
                if (!ec) {
                    element = xml_result->FirstChildElement("live_playlink_key");
                    if (element && (pText = element->GetText())) {
                        authrespone.live_playlink_key = pText;
                    } else {
                        // ec = wrong_response_packet;
                    }
                }
                if (!ec) {
                    element = xml_result->FirstChildElement("server_time");
                    if (element && (pText = element->GetText())) {
                        authrespone.server_time = parse<boost::uint64_t>(pText);
                    } else {
                        ec = wrong_response_packet;
                    }
                }

                if (!ec) {
                    element = xml_result->FirstChildElement("mutex_enable");
                    if (element && (pText = element->GetText())) {
                        authrespone.mutex_enable = parse<bool>(pText);
                    } else {
                        authrespone.mutex_enable = true;
                    }
                }

                if (!ec && authrespone.mutex_enable) {
                    element = xml_result->FirstChildElement("mutex_servers");
                    if (element) {
                        TiXmlElement * child_element = element->FirstChildElement();
                        for (; child_element != NULL; child_element = child_element->NextSiblingElement()) {
                            MutexServerInfo serverinfo;
                            serverinfo.mutex_server_ip = child_element->Attribute("ip");
                            serverinfo.http_port = atoi(child_element->Attribute("tcpport"));
                            serverinfo.udp_port = atoi(child_element->Attribute("udpport"));
                            authrespone.mutex_servers.push_back(serverinfo);
                        }
                    } else {
                        ec = wrong_response_packet;
                    }
                }

                if (!ec && authrespone.mutex_enable) {
                    element = xml_result->FirstChildElement("mutex_encrypt");
                    if (element && (pText = element->GetText())) {
                        authrespone.mutex_encrypt = pText;
                    } else {
                        ec = wrong_response_packet;
                    }
                }

                if (!ec) {
                    if (authrespone.isSuccess) {
                        if (authrespone.session_id.size() != 32)
                            ec = wrong_response_packet;
                            
                        if (authrespone.mutex_enable 
                            && (authrespone.mutex_encrypt.size() != 32 
                            || authrespone.mutex_servers.size() == 0)) {
                                ec = wrong_response_packet;
                        }
                    }
                }
            }
            if (!ec && !authrespone.isSuccess) {
                element = xml_result->FirstChildElement("reason");
                if (element && (pText = element->GetText())) {
                    authrespone.reason = atoi(pText);
                } else {
                    ec = wrong_response_packet;
                }
            }
            if (!ec && !authrespone.isSuccess) {
                ec = (ppbox::certify::error::auth_errors)(authrespone.reason + response_error_delta);
            }
            http_.response().clear_data();
            return ec;
        }

    } // namespace alive
} // namespace ppbox
