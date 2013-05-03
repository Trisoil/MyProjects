#include "stdafx.h"
#include "framework/network/HttpRequest.h"
#include "framework/network/uri.h"
#include "framework/util/strings.h"
#include <boost/format.hpp>

namespace framework
{
	namespace network
	{
		HttpRequest::p HttpRequest::ParseFromBuffer(boost::asio::streambuf buf)
		{
			string request_string;
			copy(istreambuf_iterator<char>(&buf), istreambuf_iterator<char>(), back_inserter(request_string));
			request_string += '\0';
			return ParseFromBuffer(request_string);
		}

		HttpRequest::p HttpRequest::ParseFromBuffer(string request_string)
		{
			if( string::npos == request_string.find("\r\n\r\n") )
			{	// HttpRequest 中不可能包含
				HttpRequest::p non_http_request; 
				return non_http_request;
			}

			HttpRequest::p http_request(new HttpRequest);
			http_request->request_string_ = request_string;
			vector<string> lines;
			string first_line = "";
			lines = framework::util::splite(request_string, "\r\n");

			for(vector<string>::iterator iter = lines.begin(); iter != lines.end(); iter ++)
			{
				string line = *iter;
				if( boost::algorithm::trim_copy(line).empty() )
					continue;
				if( first_line == "" )
				{
					first_line = line;
					continue;
				}
				vector<string> key_value;
				size_t splite_pos = line.find_first_of(": ");
				if( splite_pos == string::npos )
					continue;
				string key = line.substr(0, splite_pos);		
				boost::algorithm::trim(key);
				string val = line.substr(splite_pos+2, line.length()-splite_pos-2);		
				boost::algorithm::trim(val);
				http_request->header_lines_.push_back( make_pair(key, val) );
				if( boost::algorithm::to_lower_copy(key) != "pragma" ) 
				{
					if( http_request->properties_.find(key) != http_request->properties_.end() )
					{
						continue;
					}
					http_request->properties_[key] = val;
				}
				else
				{
					vector<string> pragma_key_value;
					boost::algorithm::split(pragma_key_value, line, boost::algorithm::is_any_of("="));
					if( pragma_key_value.size() != 2 ) continue;
					string pragma_key = pragma_key_value[0];		boost::algorithm::trim(pragma_key);
					string pragma_val = pragma_key_value[1];		boost::algorithm::trim(pragma_val);
					if( http_request->pragmas_.find(pragma_key) != http_request->pragmas_.end() )
					{
						continue;
					}
					http_request->pragmas_[pragma_key] = pragma_val;
				}
			}

			vector<string> str_s;
			boost::algorithm::split(str_s, first_line, boost::algorithm::is_any_of(" "));
			if( str_s.size() != 3 )
			{
				HttpRequest::p non_http_request; 
				return non_http_request;
			}
			http_request->method_ = str_s[0];						boost::algorithm::trim(http_request->method_);
			http_request->path_ = str_s[1];							boost::algorithm::trim(http_request->path_);
			http_request->version_ = str_s[2];						boost::algorithm::trim(http_request->version_);
			return http_request;
		}

		bool HttpRequest::ReSetHttpUrl()
		{
			if (path_.substr(0,4)!= "http")
			{
				path_ = "http://"+GetHost()+path_;
				return true;
			}
			return false;
		}

		string HttpRequest::GetProperty(const string& key)
		{
			if( properties_.find(key) == properties_.end() )
				return "";
			return properties_[key];
		}

		string HttpRequest::GetPragma(const string& key)
		{
			if( pragmas_.find(key) == pragmas_.end() )
				return "";
			return pragmas_[key];
		}

		string HttpRequest::GetHost()
		{
			return GetProperty("Host");
		}

		string HttpRequest::GetUrl()
		{
			string lowel_path = boost::algorithm::to_lower_copy(path_);
			if( boost::algorithm::starts_with(lowel_path, "http://") == true )
			{
				return path_;
			}
			else if ( boost::algorithm::starts_with(lowel_path, "/") == true )
			{
				string host = GetHost();
				if( host == "" ) 
					host = "localhost";
				string url = "http://";
				url += host;
				url += path_;
				return url;
			}
			else
			{
				//assert(0);
				return "";
			}
		}

		string HttpRequest::GetRefererUrl()
		{
			return GetProperty("Referer");
		}

		string HttpRequest::GetRequestString()
		{
			return request_string_;
		}

		ostream& operator<<(ostream& out, const HttpRequest& http_request)
		{
			out << http_request.GetMethod() << " " << http_request.GetPath() << " " << http_request.GetVersion() << "\r\n";
			for(map<string, string>::const_iterator iter = http_request.properties_.begin(); iter != http_request.properties_.end(); iter ++)
			{
				out << iter->first << ": " << iter->second << endl;
			}
			out << endl;
			return out;
		}

		string HttpRequestInfo::ToString()
		{
			if( http_request_demo_ )
			{
				method_ = http_request_demo_->GetMethod();
				version_ = http_request_demo_->GetVersion();
			}

			stringstream sstr;
			sstr << method_ << " " << path_ << " " << version_ << "\r\n";
			if( !http_request_demo_ )
			{	// 不是根据参考http request来
				sstr << "Accept: */*\r\n";
				if( refer_url_ != "")
					sstr << "Referer: " << refer_url_ << "\r\n";
				sstr << "x-flash-version: 9,0,28,0\r\n";
				sstr << "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;  Embedded Web Browser from: http://bsalsa.com/; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.5.20404)\r\n";
				if( host_ != "")
					sstr << "Host: " << host_ << "\r\n";
				sstr << "Connection: Keep-Alive\r\n";
			}
			else
			{	// 根据 参考http request来
				for(list<pair<string,string> >::iterator iter = http_request_demo_->header_lines_.begin();
					iter != http_request_demo_->header_lines_.end(); iter ++)
				{
					string key = (*iter).first;
					string value = (*iter).second;
					if( key == "Referer" )
					{
						if(refer_url_ != "") value = refer_url_;
						if(refer_url_.substr(0,4)!= "http")
							continue;
					}
					else if( key == "Host" )
					{
						if(host_ != "") value = host_;
					}
					else if( key == "Range" )
						continue;
					else if( key == "Content-Length")
						continue;
					else if( key == "Proxy-Connection" )
						continue;
					sstr << key << ": " << value << "\r\n";
				}
			}
			if( range_begin_ != 0 )
			{
				if( range_end_ == 0 )
					sstr << "Range: bytes=" << range_begin_ << "-\r\n";
				else
				{
					assert( range_begin_ <= range_end_);
					sstr << "Range: bytes=" << range_begin_ << "-" << range_end_ << "\r\n";
				}
			}
			else
			{
				if( range_end_ != 0 )
				{
					sstr << "Range: bytes=0-" << range_end_ << "\r\n";
				}
			}
			sstr << "\r\n";
			return sstr.str();
		}

		string HttpRequestInfo::ToFlvStartRangeString()
		{
			if( http_request_demo_ )
			{
				method_ = http_request_demo_->GetMethod();
				version_ = http_request_demo_->GetVersion();
			}
			string path = path_;
			if( range_begin_ != 0 )
			{
				assert(range_end_ == 0);
				assert(path.length() > 0);
				if( path.find("?") == string::npos )
					path.append( boost::str(boost::format("?start=%1%") % range_begin_) );
				else
				{
					assert(0);
					if (path.find("start") == string::npos)
						path.append( boost::str(boost::format("&start=%1%") % range_begin_) );
				}
			}
			stringstream sstr;
			sstr << method_ << " " << path << " " << version_ << "\r\n";

			if( !http_request_demo_ )
			{	
				sstr << "Accept: */*\r\n";
				if( refer_url_ != "")
					sstr << "Referer: " << refer_url_ << "\r\n";
				sstr << "x-flash-version: 9,0,28,0\r\n";
				sstr << "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;  Embedded Web Browser from: http://bsalsa.com/; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.5.20404)\r\n";
				if( host_ != "")
					sstr << "Host: " << host_ << "\r\n";
				
				sstr << "Connection: Keep-Alive\r\n";
			}
			else
			{
				// 根据 参考http request来
				for(list<pair<string,string> >::iterator iter = http_request_demo_->header_lines_.begin();
					iter != http_request_demo_->header_lines_.end(); iter ++)
				{
					string key = (*iter).first;
					string value = (*iter).second;
					if( key == "Referer" )
					{
						if(refer_url_ != "") value = refer_url_;
					}
					else if( key == "Host" )
					{
						if(host_ != "") value = host_;
					}
					else if( key == "Range" )
						continue;
					else if( key == "Proxy-Connection" )
						continue;
					sstr << key << ": " << value << "\r\n";
				}
			}
			sstr << "\r\n";
			return sstr.str();
		}

		string HttpRequestInfo::GetHost()
		{
			assert( !(host_=="" && ip_=="") );
			if( ip_ != "" )
				return ip_;
			if( host_ != "" )
				return host_;
			return "";
		}


		string ProxyRequestToDirectRequest(string proxy_request_string)
		{
			string direct_request_string = "";
			// 将 string 分解成一行一行
			vector<string> lines;
			bool is_first_line = true;
			lines = framework::util::splite(proxy_request_string, "\r\n");

			// 遍历一行一行，对每一行做分析
			for(vector<string>::iterator iter = lines.begin(); iter != lines.end(); iter ++)
			{
				string line = *iter;
				boost::algorithm::trim(line);
				if( line.empty() )
					continue;
				if( is_first_line == true )
				{	// 说明是第一行
					vector<string> str_s;
					boost::algorithm::split(str_s, line, boost::algorithm::is_any_of(" "));
					if( str_s.size() < 3 )
					{
						return proxy_request_string;
					}
					if( false == boost::algorithm::starts_with(boost::algorithm::to_lower_copy(str_s[1]), "http://") )
					{	// 已经是直接请求了
						direct_request_string += line + "\r\n";
					}
					else
					{
						Uri uri(str_s[1]);
						direct_request_string += str_s[0] + " " + uri.getrequest() + " " + str_s[2] + "\r\n";
					}
					is_first_line = false;
				}

				// 不是第一行
				vector<string> key_value;
				key_value = framework::util::splite(line, ": ");
				if( key_value.size() != 2 ) continue;
				string key = key_value[0];		boost::algorithm::trim(key);
				string val = key_value[1];		boost::algorithm::trim(val);
				if( key == "Proxy-Connection" )
					continue;
				direct_request_string += line;
				direct_request_string += "\r\n";
			}
			direct_request_string += "\r\n";
			return direct_request_string;
		}
	}
}

