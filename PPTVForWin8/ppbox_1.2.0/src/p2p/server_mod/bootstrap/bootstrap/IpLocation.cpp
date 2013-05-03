#include "Common.h"
#include "IpLocation.h"
#include <sstream>
#include <fstream>
#include <time.h>
#include <stdlib.h>

using namespace std;

boost::shared_ptr<IpLocation> IpLocation::inst_;

IpLocation::IpLocation(const string& filename)
{
    ifstream loadfile_stream;
    loadfile_stream.open(filename.c_str());
    if (loadfile_stream.fail())
    {
        return;
    }

    string line;
    while (getline(loadfile_stream,line))
    {
        stringstream ss(line);
        boost::uint32_t ipupper, iplower, location;
        ss >> iplower >> ipupper >> location;

        iplocation_[ipupper] = make_pair(iplower,location);
    }
}

boost::uint32_t IpLocation::GetLocation(boost::uint32_t ip)
{
    std::map<boost::uint32_t, std::pair<boost::uint32_t, boost::uint32_t> >::iterator it = iplocation_.lower_bound(ip);
    if(it == iplocation_.end() ||  it->second.first > ip)
    {
        return 0;
    }
    return it->second.second;
}

void IpLocation::SetIpLocation(const std::string & filename)
{
    inst_.reset(new IpLocation(filename));
}

boost::shared_ptr<IpLocation> IpLocation::Inst()
{
    return inst_;
}
