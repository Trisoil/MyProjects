#ifndef _GTESTFRAMEWORK_PROJECTINFO_H_
#define _GTESTFRAMEWORK_PROJECTINFO_H_

#include <string>

namespace gtestframe
{
    class ProjectInfo
    {
    public:
        struct _info
        {
            std::string pro_name;
            std::string pro_version;
            std::string pro_tester;
            std::string pro_coder;
        };
    public:
        _info init(std::string name
            ,std::string version
            ,std::string tester
            ,std::string coder
            )
        {
            info_.pro_coder = coder;
            info_.pro_name = name;
            info_.pro_tester = tester;
            info_.pro_version = version;
            return info_;
        }
        const _info& pro_info()
        {
            return info_;
        }

        static ProjectInfo* GetInstance()
        {
            if(pro_==NULL)
                pro_=new ProjectInfo;
            return pro_;
        }

        ~ProjectInfo()
        {
            if(pro_!=NULL)
                delete pro_;
        }

    private:
        _info info_;
        static ProjectInfo* pro_;
    };

   /* static ProjectInfo & _projectinfo( ... )
    {
        static ProjectInfo glog_projectinfo;
        return glog_projectinfo;
    }*/
}
//using gtestframe::_projectinfo;
#define PROJECT_DECLARE()\
    inline gtestframe::ProjectInfo::_info _prodec()\
{\
    static gtestframe::ProjectInfo::_info info = \
    gtestframe::ProjectInfo::GetInstance()->init(PROJECT_NAME,PROJECT_VERSION,TESTER,CODER);\
    return info;\
}\
static gtestframe::ProjectInfo::_info info_ = _prodec();

#endif