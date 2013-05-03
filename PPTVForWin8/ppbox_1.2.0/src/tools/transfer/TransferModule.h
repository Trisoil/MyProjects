// PlayManager.h

#ifndef _TOOLS_TRANSFER_TRANSFER_MODULE_H_
#define _TOOLS_TRANSFER_TRANSFER_MODULE_H_

#include <ppbox/mux/tool/Dispatcher.h>

namespace tools
{

    namespace transfer
    {

        class TransferModule 
            : public util::daemon::ModuleBase<TransferModule>
        {
        public:
            TransferModule(
                util::daemon::Daemon & daemon);

            virtual ~TransferModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            boost::system::error_code error() const
            {
                return error_;
            }

        private:
            void on_open(
                boost::system::error_code const & ec);

            void on_record(
                boost::system::error_code const & ec);

        private:
            size_t session_id_;
            std::string input_file_;
            std::string output_format_;
            std::string output_file_;
            ppbox::mux::Dispatcher * dispatcher_;
            boost::system::error_code error_;
        };

    } // namespace transfer
} // namespace tools

#endif // _TOOLS_TRANSFER_TRANSFER_MODULE_H_
