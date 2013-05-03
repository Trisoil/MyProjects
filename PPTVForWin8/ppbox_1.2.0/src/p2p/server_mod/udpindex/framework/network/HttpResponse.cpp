#include "stdafx.h"
#include "framework/network/HttpResponse.h"
#include "framework/util/strings.h"

//#include <boost/cast.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/split.hpp>

namespace framework
{
	namespace network
	{

		HttpResponse::p HttpResponse::ParseFromBufferMini(string response, size_t& http_header_length)
		{
			http_header_length = response.find("\r\n\r\n");
			if( string::npos ==  http_header_length)
			{	// HttpRequest 中不可能包含
				return HttpResponse::p();
			}
			http_header_length += 4;
			HttpResponse::p http_response(new HttpResponse);

			http_response->response_header_string_ = response;

			// 将 string 分解成一行一行
			vector<string> lines;
			string first_line = "";
			lines = framework::util::splite(response, "\r\n");

			// 遍历一行一行，对每一行做分析
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
				key_value = framework::util::splite(line, ": ");
				if( key_value.size() != 2 ) continue;
				string key = key_value[0];		boost::algorithm::trim(key);
				string val = key_value[1];		boost::algorithm::trim(val);
				if( boost::algorithm::to_lower_copy(key) != "pragma" ) 
				{
					if( http_response->properties_.find(key) != http_response->properties_.end() )
					{
						continue;
					}
					http_response->properties_[key] = val;
				}
				else
				{
					vector<string> pragma_key_value;
					boost::algorithm::split(pragma_key_value, line, boost::algorithm::is_any_of("="));
					if( pragma_key_value.size() != 2 ) continue;
					string pragma_key = pragma_key_value[0];		boost::algorithm::trim(pragma_key);
					string pragma_val = pragma_key_value[1];		boost::algorithm::trim(pragma_val);
					if( http_response->pragmas_.find(pragma_key) != http_response->pragmas_.end() )
					{
						continue;
					}
					http_response->pragmas_[pragma_key] = pragma_val;
				}
			}

			// 分析 第一行
			vector<string> str_s;
			boost::algorithm::split(str_s, first_line, boost::algorithm::is_any_of(" "));
			if( str_s.size() < 3 )
			{
				return HttpResponse::p();
			}
			boost::algorithm::trim(str_s[0]);
			boost::algorithm::to_upper(str_s[0]);
			if( ! boost::algorithm::starts_with( str_s[0], "HTTP") )
			{
				return HttpResponse::p();
			}
			try
			{
				http_response->status_code_	= boost::lexical_cast<u_int>(str_s[1]);
			}
			catch (...) 
			{
				return HttpResponse::p();
			}
			for(size_t i = 2; i < str_s.size(); i ++)
				http_response->status_string_.append(str_s[i] + " ");
			boost::algorithm::trim(http_response->status_string_);

			return http_response;
		}

		HttpResponse::p HttpResponse::ParseFromBuffer(string response, size_t& http_header_length)
		{
			HttpResponse::p http_response = ParseFromBufferMini(response, http_header_length);

			if( ! http_response )
				return http_response;

			// range 分析
			http_response->range_begin_ = 0;
			http_response->range_end_ = http_response->GetContentLength() - 1;
			http_response->file_length_ = http_response->GetContentLength();

			if( http_response->properties_.find("Content-Range") != http_response->properties_.end() )
			{
				do 
				{
					string content_range_string = http_response->properties_["Content-Range"];
					boost::algorithm::trim(content_range_string);
					if( false == boost::algorithm::starts_with(content_range_string, "bytes ") )
					{
						return HttpResponse::p();
					}
					content_range_string = content_range_string.substr(6);		// 滤掉 bytes 这几个字节
					boost::algorithm::trim(content_range_string);
					size_t range_end_position = content_range_string.find('/');
					if( range_end_position == string::npos )
					{
						return HttpResponse::p();
					}
					string range_string = content_range_string.substr(0, range_end_position);
					string file_length_string = content_range_string.substr(range_end_position+1);
					vector<string> range_st_s;
					boost::algorithm::split(range_st_s, range_string, boost::algorithm::is_any_of("-"));
					if(range_st_s.size() != 2)
						break;
					try { http_response->range_begin_ = boost::lexical_cast<u_int>(range_st_s[0]); } catch (...) {return HttpResponse::p();}
					try { http_response->range_end_ = boost::lexical_cast<u_int>(range_st_s[1]); } catch (...) {return HttpResponse::p();}
					try { http_response->file_length_ = boost::lexical_cast<u_int>(file_length_string); } catch (...) {return HttpResponse::p();}

				} while(false);
			}

			return http_response;
		}

		HttpResponse::p HttpResponse::ParseFromBufferByFlvStart(string response, u_int start, size_t& http_header_length)
		{
			HttpResponse::p http_response = ParseFromBufferMini(response, http_header_length);

			if ( start > 0 && http_response->GetStatusCode() == 200 )
			{
				assert(http_response->HasContentLength());
				// range 分析
				http_response->range_begin_ = start;
				http_response->file_length_ = start + http_response->GetContentLength() - 13;
			}
			else
			{
				http_response->range_begin_ = start;
				http_response->file_length_ = start + http_response->GetContentLength();
			}
			http_response->range_end_ = http_response->file_length_ - 1;
			
			return http_response;
		}
		
		string HttpResponse::GetProperty(const string& key)
		{
			if( properties_.find(key) == properties_.end() )
				return "";
			return properties_[key];
		}

		string HttpResponse::GetPragma(const string& key)
		{
			if( pragmas_.find(key) == pragmas_.end() )
				return "";
			return pragmas_[key];
		}

		bool HttpResponse::HasContentLength()
		{
			return properties_.find("Content-Length") != properties_.end();
		}

		u_int HttpResponse::GetContentLength()
		{
			string content_length_string = GetProperty("Content-Length");
			try
			{
				u_int content_length = boost::lexical_cast<u_int>(content_length_string);
				return content_length;
			}
			catch (...)
			{
				return 0;
			}
		}

		string HttpResponse::GetContentType()
		{
			return GetProperty("Content-Type");
		}

		bool HttpResponse::HasContentRange()
		{
			return properties_.find("Content-Range") != properties_.end();
		}

		bool HttpResponse::IsChunked()
		{
			if( properties_.find("Transfer-Encoding") == properties_.end() )
				return false;
			else if( properties_["Transfer-Encoding"] != "chunked" )
				return false;

			return true;
		}

		ostream& operator<<(ostream& out, const HttpResponse& http_response)
		{
			out << "HTTP " << http_response.status_code_ << " " << http_response.GetStatusString() << "\r\n";
			for(map<string, string>::const_iterator iter = http_response.properties_.begin(); iter != http_response.properties_.end(); iter ++)
			{
				out << iter->first << ": " << iter->second << endl;
			}
			out << endl;
			return out;
		}
	}
}