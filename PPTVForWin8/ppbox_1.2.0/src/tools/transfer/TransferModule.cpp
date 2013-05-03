// TransferModule.cpp

#include "tools/transfer/Common.h"
#include "tools/transfer/TransferModule.h"
#include "tools/transfer/DescriptorSink.h"
#include "tools/transfer/FileSink.h"

#include <ppbox/demux/DemuxerError.h>
#include <ppbox/demux/source/SourceError.h>
#include <framework/system/LogicError.h>
#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::logger;

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("TransferModule", 0)

namespace tools
{
    namespace transfer
    {

        TransferModule::TransferModule(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<TransferModule>(daemon, "TransferModule")
        {
            dispatcher_ = new ppbox::mux::Dispatcher(daemon);
            daemon.config().register_module("TransferModule")
                << CONFIG_PARAM_NAME_NOACC("input_file", input_file_)
                << CONFIG_PARAM_NAME_NOACC("output_format", output_format_)
                << CONFIG_PARAM_NAME_NOACC("output_file", output_file_);
        }

        TransferModule::~TransferModule()
        {
            delete dispatcher_;
        }

        void nop_resp(boost::system::error_code const & ec) {};

        boost::system::error_code TransferModule::startup()
        {
            LOG_S(Logger::kLevelEvent, "[open] input_file:"<<input_file_<<" output_format:"<<output_format_<<" output_file:"<<output_file_); 
            dispatcher_->start();
            dispatcher_->open(session_id_, input_file_, output_format_, true, 
                boost::bind(&TransferModule::on_open, this, _1));
            return boost::system::error_code();
        }

        void TransferModule::shutdown()
        {
            dispatcher_->stop();
        }

        void TransferModule::on_open(
            boost::system::error_code const & ec)
        {
            if (ec) {
                LOG_S(Logger::kLevelError, "[on_open] ec = " << ec.message());
                error_ = ec;
                dispatcher_->close(session_id_);
                get_daemon().stop(false);
                return;
            }
            //LOG_S(Logger::kLevelEvent, "[on_open] Success" << ec.message());
            std::string::size_type pos_colon = output_file_.find("://");
            std::string proto = "ppfile";
            if (pos_colon == std::string::npos) {
                pos_colon = 0;
            } else {
                proto = output_file_.substr(0, pos_colon);
                pos_colon += 3;
            }
            std::string file_name = output_file_.substr(pos_colon);
            ppbox::mux::Sink * sink = NULL;
            error_ = framework::system::logic_error::invalid_argument;
            if (proto == "ppfile") {
                sink = new FileSink(io_svc(), file_name, error_);
            } else if (proto == "ppdesc") {
                sink = new DescriptorSink(io_svc(), file_name, error_);
            }
            if (error_) {
                LOG_S(Logger::kLevelDebug, "[on_open] create sink ec = " << error_.message());
                if (sink)
                    delete sink;
                dispatcher_->close(session_id_);
                get_daemon().stop(false);
                return;
            }
            dispatcher_->setup(session_id_, sink, nop_resp);
            dispatcher_->record(session_id_, 
                boost::bind(&TransferModule::on_record, this, _1));
        }

        void TransferModule::on_record(
            boost::system::error_code const & ec)
        {
            if (ec && ec != ppbox::demux::error::no_more_sample && ec != ppbox::demux::source_error::no_more_segment) {
                LOG_S(Logger::kLevelError, "[on_record] ec:"<<ec.message());
                error_ = ec;
            }
            if(!error_) LOG_S(Logger::kLevelEvent, "[on_record] transfer success");
            dispatcher_->close(session_id_);
            get_daemon().stop(false);
        }

    } // namespace transfer
} // namespace tools
