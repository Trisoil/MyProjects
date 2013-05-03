#pragma once

namespace framework
{
	namespace network
	{
		class HttpRequestInfo;
		class HttpRequest
			: boost::noncopyable
		{
			friend struct HttpRequestInfo;
			friend ostream& operator<<(ostream& out, const HttpRequest& http_request);
		public:
			typedef boost::shared_ptr<HttpRequest> p;
			static p ParseFromBuffer(string request);
			static p ParseFromBuffer(boost::asio::streambuf buf);
		public:
			string GetMethod() const { return method_; }
			string GetPath() const { return path_; }
			string GetVersion() const { return version_; }
			string GetProperty(const string& key);
			string GetPragma(const string& key);
			string GetHost();
			string GetUrl();
			string GetRefererUrl();
			string GetRequestString();
			bool ReSetHttpUrl();///新钩挂方式中url是不带http://host的
		private:
			string method_;
			string path_;
			string version_;
			map<string, string> properties_;
			map<string, string> pragmas_;
			list<pair<string,string> > header_lines_;
			string request_string_;
		};

		ostream& operator<<(ostream& out, const HttpRequest& http_request);

		struct HttpRequestInfo
		{
			string method_;
			string domain_;
			string version_;
			string host_;
			string path_;
			string refer_url_;
			u_int range_begin_;
			u_int range_end_;
			string ip_;
			u_short port_;
			HttpRequest::p http_request_demo_;
			
			HttpRequestInfo() 
				: method_("GET"), path_("/"), domain_(""), version_("HTTP/1.0"), host_(""), ip_(""), port_(80)
				, refer_url_(""), range_begin_(0), range_end_(0) {}
			string ToString();
			string ToFlvStartRangeString();
			string GetHost();

			bool operator ! () const
			{
				return ( host_ == "" || path_ == "" || port_ == 0 );
			}
		};

		string ProxyRequestToDirectRequest(string proxy_request_string);
	}
}