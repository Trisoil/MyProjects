// MmspSession.cpp

#include "ppbox/mmspd/Common.h"
#include "ppbox/mmspd/MmspSession.h"
#include "ppbox/mmspd/MmspDispatcher.h"
#include "ppbox/mmspd/MmspdModule.h"

#include <util/protocol/mmsp/MmspMacToViewerMessage.h>
#include <util/protocol/mmsp/MmspViewerToMacMessage.h>
#include <util/protocol/mmsp/MmspError.h>
using namespace util::protocol;

#include <framework/string/Url.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Base64.h>
using namespace framework::string;

using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.mmspd.MmspSession", framework::logger::Debug)

namespace ppbox
{
    namespace mmspd
    {

        MmspSession::MmspSession(
            MmspdModule & mgr)
            : util::protocol::MmspServer(mgr.io_svc())
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
        }

        MmspSession::~MmspSession()
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

        void MmspSession::local_process(
            response_type const & resp)
        {
            boost::system::error_code ec;

            LOG_DEBUG("[local_process] session_id:" << session_id_ << " request:" << request().id());

            switch (request().id()) {
                case MmspViewerToMacMessage::CONNECT:
                    {
                        //MmspDataConnect & req(request().as<MmspDataConnect>());
                        MmspDataReportConnectedEx & rsp(response().get<MmspDataReportConnectedEx>());
                        rsp.playIncarnation = 0xf0f0f0ef; // MMS_DISABLE_PACKET_PAIR
                        rsp.cbServerVersionInfo = SERVER_VERSION_LENGTH;
                        rsp.cbAuthenPackage = AUTH_TYPE_LENGTH;
                        rsp.ServerVersionInfo = SERVER_VERSION;
                        rsp.AuthenPackage = AUTH_TYPE;
                    }
                    break;
                case MmspViewerToMacMessage::CONNECT_FUNNEL:
                    {
                        MmspDataConnectFunnel & req(request().as<MmspDataConnectFunnel>());
                        MmspDataReportConnectedFunnel & rsp(response().get<MmspDataReportConnectedFunnel>());
                        transport_ = req.funnelName.to_string();
                        rsp.funnelName = FUNNEL_NAME;
                    }
                    break;
                case MmspViewerToMacMessage::OPEN_FILE:
                    {
                        MmspDataOpenFile & req(request().as<MmspDataOpenFile>());
                        MmspDataReportOpenFile & rsp(response().get<MmspDataReportOpenFile>());
                        rsp.playIncarnation = req.playIncarnation;
                        rsp.openFileId = ++file_id_;
                        framework::string::Url url("mms://host/" + req.fileName.to_string());
                        dispatcher_ = mgr_.alloc_dispatcher(url, ec);
                        dispatcher_->async_open(url, rsp, resp);
                    }
                    return;
                case MmspViewerToMacMessage::READ_BLOCK:
                    {
                        MmspDataReadBlock & req(request().as<MmspDataReadBlock>());
                        MmspDataReportReadBlock & rsp(response().get<MmspDataReportReadBlock>());
                        rsp.playIncarnation = req.playIncarnation;
                        dispatcher_->setup(*this, transport_, ec);
                    }
                    break;
                case MmspViewerToMacMessage::STREAM_SWITCH:
                    {
                        //MmspDataStreamSwitch & req(request().as<MmspDataStreamSwitch>());
                        //MmspDataReportStreamSwitch & rsp(response().get<MmspDataReportStreamSwitch>());
                    }
                    break;
                case MmspViewerToMacMessage::START_PLAYING:
                    {
                        MmspDataStartPlaying & req(request().as<MmspDataStartPlaying>());
                        MmspDataReportStartPlaying & rsp(response().get<MmspDataReportStartPlaying>());
                        rsp.playIncarnation = req.playIncarnation;
                        rsp.tigerFileId = file_id_;
                        ++play_count_;
                        play_incarnation_ = req.playIncarnation;
                        dispatcher_->async_play(req, resp, 
                            boost::bind(&MmspSession::on_play, this, _1));

                    }
                    return;
                case MmspViewerToMacMessage::STOP_PLAYING:
                    {
                        MmspDataStopPlaying & req(request().as<MmspDataStopPlaying>());
                        MmspDataReportEndOfStream & rsp(response().get<MmspDataReportEndOfStream>());
                        rsp.playIncarnation = req.playIncarnation;
                        if (play_count_)
                            dispatcher_->cancel(ec);
                    }
                    break;
                case MmspViewerToMacMessage::CLOSE_FILE:
                    {
                        //MmspDataCloseFile & req(request().as<MmspDataCloseFile>());
                        if (dispatcher_) {
                            dispatcher_->close(ec);
                            dispatcher_ = NULL;
                        }
                    }
                    break;
                default:
                    ec = mmsp_error::unkown_command;
            }
            resp(ec);
        }

        void MmspSession::on_error(
            error_code const & ec)
        {
            LOG_INFO("[on_error] session_id:" << session_id_ << " ec:" << ec.message());
            if (dispatcher_) {
                boost::system::error_code ec1;
                dispatcher_->close(ec1);
                dispatcher_ = NULL;
            }
        }

        void MmspSession::post_process(
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

        void MmspSession::on_play(
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
                MmspMessage msg;
                MmspDataReportEndOfStream & rsp(msg.get<MmspDataReportEndOfStream>());
                rsp.playIncarnation = play_incarnation_;
                boost::system::error_code ec1;
                write(msg, ec1);
            } else if (ec != boost::asio::error::operation_aborted) {
                boost::system::error_code ec1;
                cancel(ec1);
            }
        }

        void MmspSession::on_finish()
        {
            //response().head().get_content(std::cout);

            if (request().header().MID == MmspViewerToMacMessage::READ_BLOCK) {
                boost::system::error_code ec;
                dispatcher_->write_header(*this, ec);
            } else if (request().header().MID == MmspViewerToMacMessage::START_PLAYING) {
                boost::system::error_code ec;
                dispatcher_->resume(ec);
            }
        }

    } // namespace mmspd
} // namespace ppbox
