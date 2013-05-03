#include "StdAfx.h"
#include "StunServerListConfig.h"
#include "framework/io/stdfile.h"
#include <boost/algorithm/string.hpp>
#include "protocal/IndexPacket.h"

#include "IndexFunc.h"


namespace udpindex
{
	void StunServerListConfig::LoadFile(const TCHAR *file_name)
		//��ȡStunServerList�ļ�
	{
		tstring moduleName = file_name; 
		framework::mswin::Module module;
		framework::io::Path config_path;
		module.BuildLocalFilePath(config_path, moduleName.c_str());

		framework::io::StdFileReader config_reader;
		config_reader.OpenText(config_path);

		if (config_reader.IsOpen())
		{
			string config_line;
			stun_info_.clear();
			while(config_reader.ReadLine(config_line))
			{
				boost::algorithm::trim(config_line);
				if ( config_line.empty() == true || config_line[0] == '#' )
				{
					continue;
				}
				else
				{
					STUN_SERVER_INFO ds;
					bool is_succed = false;
					try
					{
						vector<string> tracker_s;
						char split_char_s[] = " \t\r\n";
						char ip_split_char_s[] = ".";
						boost::algorithm::split(tracker_s, config_line, boost::algorithm::is_any_of(split_char_s));

						int pos = 0;

						for(vector<string>::iterator iter = tracker_s.begin();
							iter != tracker_s.end(); iter ++)
						{
							if( boost::algorithm::trim_copy(*iter).empty() == true )
							{
								continue;
							}
							else if( pos == 0 )
							{
								if(boost::algorithm::to_lower_copy(*iter) == "udp")
								{
									ds.Type = 1;
								}
								else
								{
									//?
									ds.Type = 0;
								}
								pos ++;
							}
							else if( pos == 1 )
							{
								boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(*iter);
								ds.IP = addr.to_ulong();
								pos ++;
							}
							else if( pos == 2 )
							{
								ds.Port = boost::lexical_cast<u_short>(*iter);
								is_succed = true;
								break;
							}
						}	// end for
					}
					catch (boost::bad_lexical_cast&) 
					{
						assert(0);
					}
					if( is_succed == true )
					{
						stun_info_.push_back(ds);
					}
					else
					{
						CONSOLE_LOG("There\'s an error line in the stun server list file!");
					}
				}	// end if-else
			}	// end while
		}
		else
		{
			CONSOLE_LOG("StunServerListConfig::LoadFile() Failed to Open the stun server List File");
		}
	}

	QueryStunServerListResponsePacket::p StunServerListConfig::GetStunServerListPacket()
	{
		QueryStunServerListResponsePacket::p response_packet = 
			QueryStunServerListResponsePacket::CreatePacket( stun_info_);
		return response_packet;
	}
}