#ifndef _PTELNET_ARGUMENTPARSE_H_
#define _PTELNET_ARGUMENTPARSE_H_

#include <map>
#include <string>

namespace ptelnet
{
    class ArgumentParse
    {
    public:
        struct ArgumentInfo
        {
            ArgumentInfo(bool a)
            {
                auto_ = a;
            }
            bool auto_;
            std::map<std::string ,std::string> info_;
        };
        ArgumentParse(int argc, char** argv)
            :argc_(argc)
            ,argv_(argv)
            ,info_(false)
        {
            
        }

        const ArgumentInfo& argument_parse()
        {
            for(int i=1; i<argc_; i++)
            {
                std::string value = argv_[i];
                if( value.find('-') == 0 )
                {
                    std::string key = value.substr(1);
                    std::string keyvalue = argv_[i+1];
                    info_.auto_ = true;
                    if(!key.compare("f"))
                    {
                        keyvalue.insert(0, "sh ");
                        keyvalue.push_back('\r');
                        keyvalue.push_back('\n');
                    }
                    info_.info_[key] = keyvalue;
                    i++;
                }
                else
                {
                    if(i == 1)
                    {
                        info_.info_["u"] = value;
                        info_.info_["p"] = "23";
                    }
                    else
                    {
                        info_.info_["p"] = value;
                    }
                }
            }
            return info_;
        }

    private:
        int argc_;
        char** argv_;
        ArgumentInfo info_;
    };
} //namespace ptelnet

#endif //_PTELNET_ARGUMENTPARSE_H_