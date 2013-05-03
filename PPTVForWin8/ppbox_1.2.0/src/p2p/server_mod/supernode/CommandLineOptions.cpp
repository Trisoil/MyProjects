//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "CommandLineOptions.h"

namespace super_node
{
    const string CommandLineOptions::SupportedOptions::Help = "help";
    const string CommandLineOptions::SupportedOptions::Version = "version";
    const string CommandLineOptions::SupportedOptions::Background = "background";

    CommandLineOptions::CommandLineOptions()
        : options_desciption_("Allowed options")
    {
        show_help_flag_ = false;
        show_version_flag_ = false;
        run_as_background_ = false;

        options_desciption_.add_options()
            (SupportedOptions::Help.c_str(), "help message")
            (SupportedOptions::Version.c_str(), "service version")
            (SupportedOptions::Background.c_str(), "run service in the background");
    }

    bool CommandLineOptions::TryParse(int argc, char* argv[])
    {
        po::variables_map vm;

        try
        {
            po::store(po::parse_command_line(argc, argv, options_desciption_), vm);
            po::notify(vm);
        }
        catch (boost::program_options::error & e)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "An error occurred while parsing the command line arguments:" << e.what());
            OperationsLogging::Log(OperationsLogEvents::ErrorParsingCommandLineArguments, Error);
            std::cout<<"Bad arguments:"<<e.what()<<std::endl;
            return false;
        }

        show_help_flag_ = vm.count(SupportedOptions::Help) > 0;
        show_version_flag_ = vm.count(SupportedOptions::Version) > 0;
        run_as_background_ = vm.count(SupportedOptions::Background) > 0;

        return true;
    }
}