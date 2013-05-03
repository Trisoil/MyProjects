// UrlInfo.h

#ifndef _STORAGE_URL_INFO_H_
#define _STORAGE_URL_INFO_H_

namespace storage
{

    struct UrlInfo
    {
        boost::uint8_t type_;
        string url_;
        string refer_url_;
        string identifier_;

        UrlInfo()
            : type_(0)
        {
        }

        UrlInfo(
            string const & url, 
            string const & refer)
            : type_(0)
            , url_(url)
            , refer_url_(refer)
            , identifier_(url)
        {
        }

        enum URL_INFO_TYPE
        {
            HTTP = 0,
            FLASH_MEDIA = 1
        };

        friend bool operator < (
            const UrlInfo& l, 
            const UrlInfo& r)
        {
            if( l.type_ != r.type_ )
                return l.type_ < r.type_;
            return l.url_ < r.url_;
            //else return refer_url_ < n.refer_url_;
        }

        friend std::ostream & operator << (
            std::ostream& os, 
            const UrlInfo& url_info)
        {
            os << "(" << url_info.type_ << ", " << url_info.url_ << ", " << url_info.refer_url_ << ")";
            return os;
        }

        string GetIdentifiableUrl()
        {
            // youtube
            if (url_.find("googlevideo.com/get_video?video_id=") != string::npos) {
                string::size_type i = url_.find("&signature=");
                if (i != string::npos)
                    return url_.substr(0, i);
            }
            return url_;
        }
    };

}
#endif // _STORAGE_URL_INFO_H_
