#pragma once

namespace framework
{
	namespace network
	{
		class HttpResponse
			: boost::noncopyable
		{
			friend ostream& operator<<(ostream& out, const HttpResponse& http_response);
		public:
			typedef boost::shared_ptr<HttpResponse> p;
			static p ParseFromBuffer(string response, size_t& http_header_length);
			static p ParseFromBufferByFlvStart(string response, u_int start, size_t& http_header_length);
		protected:
			static p ParseFromBufferMini(string response, size_t& http_header_length);
		public:
			u_int GetStatusCode() const { return status_code_; }
			string GetStatusString() const { return status_string_; }
			string GetProperty(const string& key);
			string GetPragma(const string& key);
			// Content-Type
			string GetContentType();
			// Content-Length
			bool HasContentLength();
			u_int GetContentLength();
			u_int GetFileLength() {return file_length_; }
			// Content-Range
			bool HasContentRange();
			u_int GetRangeBegin() const { return range_begin_; }
			u_int GetRangeEnd() const { return range_end_; }
			// 
			bool IsChunked();
			string ToString() { return response_header_string_; }
		private:
			string response_header_string_;
			u_int status_code_;
			string status_string_;
			map<string, string> properties_;
			map<string, string> pragmas_;
			size_t range_begin_;
			size_t range_end_;
			size_t file_length_;
		};

		ostream& operator<<(ostream& out, const HttpResponse& http_response);
	}
}