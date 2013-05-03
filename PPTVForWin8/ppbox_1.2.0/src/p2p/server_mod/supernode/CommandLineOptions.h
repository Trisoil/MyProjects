//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_COMMAND_LINE_OPTIONS_H
#define SUPER_NODE_COMMAND_LINE_OPTIONS_H

#include <boost/program_options.hpp>

namespace super_node
{
    namespace po = boost::program_options;

    class CommandLineOptions
    {
        class SupportedOptions
        {
        public:
            static const string Help;
            static const string Version;
            static const string Background;
        };

    public:
        CommandLineOptions();

        bool TryParse(int argc, char* argv[]);

        bool ShouldShowHelp() const
        {
            return show_help_flag_;
        }

        bool ShouldShowVersion() const
        {
            return show_version_flag_;
        }

        bool RunAsBackground() const
        {
            return run_as_background_;
        }

        void ShowHelp()
        {
            std::cout<<options_desciption_<<std::endl;
        }

    private:
        po::options_description options_desciption_;
        bool show_help_flag_;
        bool show_version_flag_;
        bool run_as_background_;
    };
}

#endif //SUPER_NODE_COMMAND_LINE_OPTIONS_H