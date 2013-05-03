// LogSubmitter.cpp

#include "ppbox/dac/Common.h"
#include "ppbox/dac/LogSubmitter.h"
#include "ppbox/dac/DacType.h"

#include <util/protocol/pptv/Base64.h>
#include <util/protocol/http/HttpRequest.h>
using namespace util::protocol;

#include <framework/string/Compress.h>
#include <framework/string/Format.h>
#include <framework/filesystem/Path.h>
#include <framework/network/NetName.h>
#include <framework/timer/TimeCounter.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/memory/MemoryPage.h>
using namespace framework::network;

#include <boost/filesystem/operations.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>
using namespace boost::system;

#include <fstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.dac.LogSubmitter", framework::logger::Trace);

namespace ppbox
{
    namespace dac
    {
        struct Buffer
        {
            Buffer(
                boost::uint32_t len)
                : size(framework::memory::MemoryPage::align_page(len))
                , buf((char *)malloc(size))
                , used(0)
            {
            }

            ~Buffer()
            {
                if (buf) {
                    free(buf);
                    used = 0;
                }
                size = 0;
            }

            boost::uint32_t size;
            char * buf;
            boost::uint32_t used;
        };

        LogSubmitter::LogSubmitter()
        {
            boost::filesystem::path temp_path = 
                framework::filesystem::temp_path();
            ppbox_log_ = (temp_path / "ppbox.log").file_string();
            alive_log_ = (temp_path / "ppbox_alive.log").file_string();
            vod_log_   = (temp_path / "vod_worker.log").file_string();
            live_log_  = (temp_path / "live_worker.log").file_string();
        }

        LogSubmitter::~LogSubmitter()
        {
        }

        bool LogSubmitter::get_request(
            boost::asio::streambuf & buf,
            std::string const & msg)
        {
            bool res = false;
            Buffer log_buffer(500 * 1024);
            framework::timer::TimeCounter tc;
            if (join_log(log_buffer) && (msg.empty() || join_ui_log(log_buffer, msg))) {
                // seed log
                char * p_buf = const_cast<char *>(boost::asio::buffer_cast<char const *>(buf.prepare(log_buffer.used)));
                size_t length = framework::string::compress(log_buffer.buf, log_buffer.used, p_buf);
                if (tc.elapse() > 50) {
                    LOG_DEBUG("[get_request] compress elapse: " << tc.elapse());
                }
                buf.commit(length);

                LOG_DEBUG("[get_request] success ");

                res = true;
            }

            return res;
        }

        bool LogSubmitter::join_log(
            Buffer & log_buffer)
        {
            bool is_write = false;
            log_buffer.used = 0;
            // join ppbox.log file
            if (file_to_buffer(log_buffer, ppbox_log_)) {
                is_write = true;
            }
            // join ppbox_alive.log file
            if (file_to_buffer(log_buffer, alive_log_)) {
                is_write = true;
            }
            // join vod_worker.log file
            if (file_to_buffer(log_buffer, vod_log_)) {
                is_write = true;
            }
            // join live_worker.log file
            if (file_to_buffer(log_buffer, live_log_)) {
                is_write = true;
            }
            return is_write;
        }

        bool LogSubmitter::join_ui_log(
            Buffer & log_buffer, 
            std::string const & msg)
        {
            std::string prefix = "\nui.log\n";
            if (log_buffer.size > log_buffer.used + prefix.size()) {
                memcpy(log_buffer.buf + log_buffer.used, prefix.c_str(), prefix.size());
                log_buffer.used += prefix.size();
            }

            if (log_buffer.size > log_buffer.used + msg.size()) {
                memcpy(log_buffer.buf + log_buffer.used, msg.c_str(), msg.size());
                log_buffer.used += msg.size();
            } else {
                LOG_WARN("[join_ui_log] Low buffer save log file ");
            }

            return true;
        }

        bool LogSubmitter::file_to_buffer(
            Buffer & log_buffer, 
            std::string const & filename)
        {
            bool res = false;
            std::string prefix = "\n";
            if (boost::filesystem::exists(filename)) {
                std::ifstream is;
                is.open(filename.c_str(), std::ios::binary);
                if (is.is_open()) {
                    prefix += filename;
                    prefix += "\n";

                    if (log_buffer.size > log_buffer.used + prefix.size()) {
                        memcpy(log_buffer.buf + log_buffer.used, prefix.c_str(), prefix.size());
                        log_buffer.used += prefix.size();

                        boost::uint32_t length = file_size(is);
                        if (log_buffer.size > log_buffer.used + length) {
                            is.read(log_buffer.buf + log_buffer.used, length);
                            log_buffer.used += length;
                            res = true;
                        } else {
                            LOG_WARN("[file_to_buffer] Low buffer save log file ");
                        }
                    }
                } else {
                    LOG_WARN("[file_to_buffer] Log File not found: " << filename);
                }
                is.close();
            }
            return res;
        }

        boost::uint32_t LogSubmitter::file_size(
            std::ifstream & is)
        {
            boost::uint32_t length = 0;
            assert(is.is_open());
            is.seekg(0, std::ios::end);
            length = is.tellg();
            is.seekg(0, std::ios::beg);
            return length;
        }

    }
}
