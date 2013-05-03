// RtmpSession.cpp

#include "ppbox/rtmpd/Common.h"
#include "ppbox/rtmpd/RtmpSession.h"
#include "ppbox/rtmpd/RtmpDispatcher.h"
#include "ppbox/rtmpd/RtmpdModule.h"

#include <util/protocol/rtmp/RtmpMessage.hpp>
#include <util/protocol/rtmp/RtmpMessageDataCommand.h>
#include <util/protocol/rtmp/RtmpError.h>
using namespace util::protocol;

#include <framework/string/Url.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Base64.h>
using namespace framework::string;

using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtmpd.RtmpSession", framework::logger::Debug)

namespace ppbox
{
    namespace rtmpd
    {

        RtmpSession::RtmpSession(
            RtmpdModule & mgr)
            : util::protocol::RtmpServer(mgr.io_svc())
            , mgr_(mgr)
            , session_id_(0)
            , dispatcher_(NULL)
            , file_id_(0)
            , play_count_(0)
            , play_incarnation_(0)
        {
            static boost::uint32_t g_id = 0;
            session_id_ = ++g_id;

            boost::system::error_code ec;
            set_non_block(true, ec);

            RtmpAmfValue v;
            RtmpAmfObject & o = v.get<RtmpAmfObject>();
            o.ObjectProperties.push_back(RtmpAmfObjectProperty("fmsVer", "FMS/3,5,3,824"));
            o.ObjectProperties.push_back(RtmpAmfObjectProperty("capabilities", 127));
            {
                RtmpAmfValue v2(v);
            }
        }

        RtmpSession::~RtmpSession()
        {
            if (dispatcher_) {
                mgr_.free_dispatcher(dispatcher_);
                dispatcher_ = NULL;
            }
        }

#define SERVER_VERSION L"9.01.01.3841"
#define SERVER_VERSION_LENGTH (sizeof(SERVER_VERSION) / 2)

#define AUTH_TYPE L"NTLM"
#define AUTH_TYPE_LENGTH (sizeof(AUTH_TYPE) / 2)

#define FUNNEL_NAME L"Funnel Of The Gods"

        template <size_t n>
        static std::vector<boost::uint16_t> wstr_to_vec(
            wchar_t const (&str)[n])
        {
            return std::vector<boost::uint16_t>(str, str + n);
        }

        void RtmpSession::local_process(
            response_type const & resp)
        {
            boost::system::error_code ec;

            RtmpCommandMessage & req(
                request().type == RtmpMessageType::RCMT_CommandMessage0 
                    ? (RtmpCommandMessage &)request().as<RtmpCommandMessage0>()
                    : (RtmpCommandMessage &)request().as<RtmpCommandMessage3>());
            std::string const & cmd = req.CommandName.as<RtmpAmfString>().StringData;
            cmd_ = cmd;

            LOG_DEBUG("[local_process] session_id:" << session_id_ << " request:" << cmd);

            if (cmd == "connect") {
                path_ = req.CommandObject.as<RtmpAmfObject>()["tcUrl"].as<RtmpAmfString>().StringData;
                path_ += "/";

                RtmpMessage resp;
                resp.chunk = request().chunk; // 3;
                RtmpCommandMessage & result = resp.get<RtmpCommandMessage0>();
                result.CommandName = "_result";
                result.TransactionID = req.TransactionID;
                {
                    RtmpAmfObject & obj = result.CommandObject.get<RtmpAmfObject>();
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("fmsVer", "FMS/3,5,3,824"));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("capabilities", 127));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("mode", 1));
                }
                result.OptionalArguments.push_back(RtmpAmfValue());
                {
                    RtmpAmfObject & obj = result.OptionalArguments.back().get<RtmpAmfObject>();
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("level", "status"));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("code", "NetConnection.Connect.Success"));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("description", "Connection succeeded."));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("objectEncoding", 3));
                    obj.ObjectProperties.push_back(RtmpAmfObjectProperty("data", 3));
                    RtmpAmfECMAArray & arr = obj.ObjectProperties.back().PropertyData.get<RtmpAmfECMAArray>();
                    arr.ECMAArrayLength = 1;
                    arr.Variables.push_back(RtmpAmfObjectProperty("version", "3,5,3,824"));
                }
                response().push_back(resp);
            } else if (cmd == "createStream") {
                RtmpMessage resp;
                resp.chunk = request().chunk; // 3;
                RtmpCommandMessage & result = resp.get<RtmpCommandMessage0>();
                result.CommandName = "_result";
                result.TransactionID = req.TransactionID;
                result.CommandObject.reset(RtmpAmfType::_NULL); 
                result.OptionalArguments.push_back(1);
                response().push_back(resp);
            } else if (cmd == "play") {
                std::string const & file = req.OptionalArguments.front().as<RtmpAmfString>().StringData;
                path_ += file;

                framework::string::Url url(path_);
                dispatcher_ = mgr_.alloc_dispatcher(url, ec);
                ++play_count_;
                dispatcher_->async_open_play(*this, url, resp, 
                    boost::bind(&RtmpSession::on_play, this, _1));

                RtmpMessage resp;
                resp.chunk = 4;
                resp.stream = 1;
                RtmpCommandMessage & result = resp.get<RtmpCommandMessage0>();
                result.CommandName = "onStatus";
                result.TransactionID = req.TransactionID;
                result.CommandObject.reset(RtmpAmfType::_NULL); 
                result.OptionalArguments.push_back(RtmpAmfValue());
                RtmpAmfObject & obj = result.OptionalArguments.back().get<RtmpAmfObject>();
                obj.ObjectProperties.push_back(RtmpAmfObjectProperty("level", "status"));
                obj.ObjectProperties.push_back(RtmpAmfObjectProperty("code", "NetStream.Play.Reset"));
                obj.ObjectProperties.push_back(RtmpAmfObjectProperty("description", "Playing and resetting."));
                obj.ObjectProperties.push_back(RtmpAmfObjectProperty("details", file));
                obj.ObjectProperties.push_back(RtmpAmfObjectProperty("clientid", "MmWA8ugO"));
                response().push_back(resp);
                obj.ObjectProperties[1].PropertyData = "NetStream.Play.Start";
                obj.ObjectProperties[2].PropertyData = "Started playing.";
                response().push_back(resp);

                return;
            } else if (cmd == "seek") {
            } else if (cmd == "pause") {
            } else if (cmd == "deleteStream") {
            } else if (cmd == "close") {
            }
            /*
                case RtmpViewerToMacMessage::OPEN_FILE:
                    {
                        RtmpDataOpenFile & req(request().as<RtmpDataOpenFile>());
                        RtmpDataReportOpenFile & rsp(response().get<RtmpDataReportOpenFile>());
                        rsp.playIncarnation = req.playIncarnation;
                        rsp.openFileId = ++file_id_;
                        framework::string::Url url("mms://host/" + req.fileName.to_string());
                        dispatcher_ = mgr_.alloc_dispatcher(url, ec);
                        dispatcher_->async_open(url, rsp, resp);
                    }
                    return;
                case RtmpViewerToMacMessage::READ_BLOCK:
                    {
                        RtmpDataReadBlock & req(request().as<RtmpDataReadBlock>());
                        RtmpDataReportReadBlock & rsp(response().get<RtmpDataReportReadBlock>());
                        rsp.playIncarnation = req.playIncarnation;
                        dispatcher_->setup(*this, transport_, ec);
                    }
                    break;
                case RtmpViewerToMacMessage::STREAM_SWITCH:
                    {
                        //RtmpDataStreamSwitch & req(request().as<RtmpDataStreamSwitch>());
                        //RtmpDataReportStreamSwitch & rsp(response().get<RtmpDataReportStreamSwitch>());
                    }
                    break;
                case RtmpViewerToMacMessage::START_PLAYING:
                    {
                        RtmpDataStartPlaying & req(request().as<RtmpDataStartPlaying>());
                        RtmpDataReportStartPlaying & rsp(response().get<RtmpDataReportStartPlaying>());
                        rsp.playIncarnation = req.playIncarnation;
                        rsp.tigerFileId = file_id_;
                        ++play_count_;
                        play_incarnation_ = req.playIncarnation;
                        dispatcher_->async_play(req, resp, 
                            boost::bind(&RtmpSession::on_play, this, _1));

                    }
                    return;
                case RtmpViewerToMacMessage::STOP_PLAYING:
                    {
                        RtmpDataStopPlaying & req(request().as<RtmpDataStopPlaying>());
                        RtmpDataReportEndOfStream & rsp(response().get<RtmpDataReportEndOfStream>());
                        rsp.playIncarnation = req.playIncarnation;
                        if (play_count_)
                            dispatcher_->cancel(ec);
                    }
                    break;
                case RtmpViewerToMacMessage::CLOSE_FILE:
                    {
                        //RtmpDataCloseFile & req(request().as<RtmpDataCloseFile>());
                        if (dispatcher_) {
                            dispatcher_->close(ec);
                            dispatcher_ = NULL;
                        }
                    }
                    break;
                default:
                    ec = rtmp_error::unkown_command;
            }*/
            resp(ec);
        }

        void RtmpSession::on_error(
            error_code const & ec)
        {
            LOG_INFO("[on_error] session_id:" << session_id_ << " ec:" << ec.message());
            if (dispatcher_) {
                boost::system::error_code ec1;
                dispatcher_->close(ec1);
                dispatcher_ = NULL;
            }
        }

        void RtmpSession::post_process(
            response_type const & resp)
        {
            LOG_INFO("[post_process] session_id:" << session_id_);

            if (play_count_) {
                post_resp_ = resp;
            } else {
                boost::system::error_code ec;
                resp(ec);
            }
        }

        void RtmpSession::on_play(
            boost::system::error_code const & ec)
        {
            LOG_INFO("[on_play] session_id:" << session_id_ << " ec:" << ec.message());

            --play_count_;

            if (!post_resp_.empty() && play_count_ == 0) {
                boost::system::error_code ec1;
                response_type resp;
                resp.swap(post_resp_);
                resp(ec1);
            } else if (!ec) {
                RtmpMessage msg;
                /*
                RtmpDataReportEndOfStream & rsp(msg.get<RtmpDataReportEndOfStream>());
                rsp.playIncarnation = play_incarnation_;
                boost::system::error_code ec1;
                write(msg, ec1);*/
            } else if (ec != boost::asio::error::operation_aborted) {
                boost::system::error_code ec1;
                cancel(ec1);
            }
        }

        void RtmpSession::on_finish()
        {
            //response().head().get_content(std::cout);

            if (cmd_ == "play") {
                set_write_parallel(true);
                boost::system::error_code ec;
                dispatcher_->resume(ec);
            }
        }

    } // namespace rtmpd
} // namespace ppbox
