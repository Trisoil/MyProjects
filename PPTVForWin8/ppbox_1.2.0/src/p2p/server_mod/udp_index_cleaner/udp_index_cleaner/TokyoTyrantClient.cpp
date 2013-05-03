#include "StdAfx.h"
#include "TokyoTyrantClient.h"

namespace TokyoTyrant
{

    TokyoTyrantClient::p TokyoTyrantClient::Create(const string &ip_str, const short port)
    {
        return p(new TokyoTyrantClient(ip_str, port));
    }


    bool TokyoTyrantClient::Get(const string &key, string &val)
    {
        if (!is_running_) return false;
        val = "";
        boost::asio::streambuf m_request;
        ostream oss(&m_request);
        oss << "GET /" << key
            << " HTTP/1.1\r\n"
            << "Host: "
            << host_.ip_
            << ":"
            << host_.port_
            << "\r\n"
            << "Accept: */*\r\n"
            << "\r\n";

        boost::asio::write(socket_, m_request, boost::asio::transfer_all(), error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }


        boost::asio::streambuf m_response;
        boost::asio::read_until(socket_, m_response, "\r\n\r\n", error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        val = "";
        size_t val_len = 0; 
        bool is_ok = false;
        istream iss(&m_response);
        string line;
        while (getline(iss, line) && line[0] != '\r') {
            if (!is_ok && line.find("OK") != string::npos)
            {
                is_ok = true;
            }
            if (line.find("Content-Length: ") == 0)
            {
                val_len = atoi(line.substr(strlen("Content-Length: ")).c_str());
                if (val_len <= 0)
                {
                    return false;
                }
            }
        }

        val_len -= m_response.size();

        copy(istreambuf_iterator<char>(&m_response), istreambuf_iterator<char>(), back_inserter(val));

        if (val_len > 0)
        {
            boost::asio::read(socket_, m_response, boost::asio::transfer_at_least(val_len), error_code_);
            if (error_code_) 
            {
                is_running_ = false;
                return false;
            }

            copy(istreambuf_iterator<char>(&m_response), istreambuf_iterator<char>(), back_inserter(val));   
        }
        return is_ok;    
    }
    bool TokyoTyrantClient::Set(const string &key, const string &val)
    {
        if (!is_running_) return false;

        boost::asio::streambuf m_request;
        boost::asio::streambuf m_response;
        ostream oss(&m_request);
        oss << "PUT /" << key
            << " HTTP/1.1\r\n"
            << "Host: "
            << host_.ip_
            << ":"
            << host_.port_
            << "\r\n"
            << "Accept: */*\r\n"
            << "Content-Length: "
            << val.length()
            << "\r\n"
            << "\r\n"
            << val;

        boost::asio::write(socket_, m_request, boost::asio::transfer_all(), error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        boost::asio::read_until(socket_, m_response, "\r\n\r\n", error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        string val_tmp = "";
        size_t val_len = 0; 
        bool is_ok = false;
        istream iss(&m_response);
        string line;
        while (getline(iss, line) && line[0] != '\r') {
            if (!is_ok && line.find("Created") != string::npos)
            {
                is_ok = true;
            }
            if (line.find("Content-Length: ") == 0)
            {
                val_len = atoi(line.substr(strlen("Content-Length: ")).c_str());
                if (val_len <= 0)
                {
                    return false;
                }
            }
        }

        val_len -= m_response.size();
        if (val_len > 0)
        {
            boost::asio::read(socket_, m_response, boost::asio::transfer_at_least(val_len), error_code_);
            if (error_code_) 
            {
                is_running_ = false;
                return false;
            }
        }
        return is_ok;    
    }
    bool TokyoTyrantClient::Del(const string &key)
    {
        if (!is_running_) return false;

        boost::asio::streambuf m_request;
        boost::asio::streambuf m_response;
        ostream oss(&m_request);
        oss << "DELETE /" << key
            << " HTTP/1.1\r\n"
            << "Host: "
            << host_.ip_
            << ":"
            << host_.port_
            << "\r\n"
            << "Accept: */*\r\n"
            << "\r\n";

        boost::asio::write(socket_, m_request, boost::asio::transfer_all(), error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        boost::asio::read_until(socket_, m_response, "\r\n\r\n", error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        string val_tmp = "";
        size_t val_len = 0; 
        bool is_ok = false;
        istream iss(&m_response);
        string line;
        while (getline(iss, line) && line[0] != '\r') {
            if (!is_ok && (line.find("OK") != string::npos || line.find("Not Found") != string::npos))
            {
                is_ok = true;
            }
            if (line.find("Content-Length: ") == 0)
            {
                val_len = atoi(line.substr(strlen("Content-Length: ")).c_str());
                if (val_len <= 0)
                {
                    return false;
                }
            }
        }

        val_len -= m_response.size();
        if (val_len > 0)
        {
            boost::asio::read(socket_, m_response, boost::asio::transfer_at_least(val_len), error_code_);
            if (error_code_) 
            {
                is_running_ = false;
                return false;
            }
        }
        return is_ok;    
    }

    bool TokyoTyrantClient::Stat(const string &key, string &val)
    {
        if (!is_running_) return false;

        boost::asio::streambuf m_request;
        boost::asio::streambuf m_response;
        ostream oss(&m_request);
        oss << "stats"
            << "\r\n";

        boost::asio::write(socket_, m_request, boost::asio::transfer_all(), error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        boost::asio::read_until(socket_, m_response, "END\r\n", error_code_);
        if (error_code_) 
        {
            is_running_ = false;
            return false;
        }

        istream iss(&m_response);
        string line;
        size_t ps = 0;
        while (getline(iss, line) && (ps = line.find(key)) == string::npos);
        if (ps != string::npos)
        {
            line = line.substr(ps+key.length());
            val = line.substr(1,line.length()-2);
            return true;
        }

        return false;
    }

    bool TokyoTyrantClient::Connect(const string &ip_str, const short port)
    {
        if (is_running_) return true;

        boost::asio::ip::address_v4 adv4 = boost::asio::ip::address_v4::from_string(ip_str, error_code_);
        if (error_code_) return false;

        end_point_ = boost::asio::ip::tcp::endpoint(adv4, port);
        host_.ip_ = ip_str;
        host_.port_ = port;

        socket_.connect(end_point_, error_code_);
        if (error_code_) return false;

        is_running_ = true;
        return true;
    }
    bool TokyoTyrantClient::Disconnect()
    {
        if (!is_running_) return true;
        is_running_ = false;
        socket_.close(error_code_);
        if (error_code_)return false;

        return true;
    }


}
