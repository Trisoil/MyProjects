#include "stdafx.h"

#include <boost/asio.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <string>

#include "framework/io/stdfile.h"

#include "config.h"

using namespace std;
using namespace framework::io;

namespace framework
{
    Config::p Config::inst_(new Config());

    bool Config::LoadConfig(const tstring& filename)
    {
        framework::io::StdFileReader file;

        file.OpenText(filename.c_str());

        if (false == file.IsOpen())
            return false;

        string line;
        while (file.ReadLine(line))
        {
            // empty line or comments
            size_t comment_index = line.find_first_of('#');
            if (comment_index != string::npos)
                line = line.substr(0, comment_index);

            line = boost::algorithm::trim_left_copy( line );
            if (line.length() == 0 || line[0] == '#')
                continue;

            string key, value;
            size_t index = line.find_first_of('=');
            if (index != string::npos)
            {
                key = boost::algorithm::trim_copy( line.substr(0, index) );
                value = boost::algorithm::trim_copy( line.substr(index + 1) );

                // store
                configs_[key] = value;
            }
        }
        return true;
    }
}
