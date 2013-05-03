#ifndef _PTELNET_LOGINPARSE_H_
#define _PTELNET_LOGINPARSE_H_

#include <vector>
#include <string>
#include <tinyxml/tinyxml.h>

namespace ptelnet
{
    class LoginParse
    {
    public:
        struct command
        {
            command(std::string key, std::string v)
            {
                key_ = key;
                value_ = v;
            }
            std::string key_;
            std::string value_;
        };
        struct LoginInfo
        {
            std::string ip_;
            std::string port_;
            std::vector<command> cmds_;
        };

        LoginParse(std::string loginpath
            ,std::string initpath)
            :loginpath_(loginpath)
            ,initpath_(initpath)
        {

        }

        ~LoginParse()
        {
            if(doc_ != NULL)
                delete doc_;
        }

        const LoginInfo& login_parse()
        {
            doc_ = new TiXmlDocument;
            doc_->LoadFile(loginpath_.c_str(), TIXML_ENCODING_UNKNOWN);
            TiXmlElement* root = doc_->RootElement();
            TiXmlNode* ip = root->FirstChild();
            TiXmlNode* port = ip->NextSibling();
            info_.ip_ = ip->FirstChild()->Value();
            TiXmlNode* portvalue = port->FirstChild();
            if(portvalue)
                info_.port_ = port->FirstChild()->Value();
            else
                info_.port_ = "23";
            TiXmlNode* cmds = port->NextSibling();
            TiXmlNode* cmd = cmds->FirstChild();
            while (cmd)
            {
                TiXmlElement* element = cmd->ToElement();
                if(element)
                {
                    std::string v = element->Attribute("value");
                    v.push_back('\r');
                    v.push_back('\n');
                    info_.cmds_.push_back(command(element->Attribute("key"), v));
                }
                cmd = cmd->NextSibling();
            }
            doc_->Clear();
            if(initpath_.size() > 0 )
            {
                doc_->LoadFile(initpath_.c_str(), TIXML_ENCODING_UNKNOWN);
                TiXmlElement* root = doc_->RootElement();
                TiXmlNode* icmd = root->FirstChild();
                while(icmd)
                {
                    TiXmlElement* ielement = icmd->ToElement();
                    if(ielement)
                    {
                        std::string ik = ielement->Attribute("key");
                        std::string iv = ielement->Attribute("value");
                        iv.push_back('\r');
                        iv.push_back('\n');
                        if(ik.size() <= 0)
                        {
                            ik = info_.cmds_.back().key_;
                        }
                        info_.cmds_.push_back(command(ik, iv));
                    }
                    icmd = icmd->NextSibling();
                }
            }
            return info_;
        }

    private:
        std::string loginpath_;
        std::string initpath_;
        TiXmlDocument* doc_;
        LoginInfo info_;
    };
} //namespace ptelnet

#endif //_PTELNET_LOGINPARSE_H_