//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_IDENTIFIER_H
#define SUPER_NODE_RESOURCE_IDENTIFIER_H

#include "count_object_allocate.h"

namespace super_node
{
    //BKDR Hash Function
    inline unsigned int BKDRHash(const char *str)
    {
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        while (*str)
        {
            hash = hash * seed + (*str++);
        }

        return (hash & 0x7FFFFFFF);
    }

    class ResourceIdentifier
        : public count_object_allocate<ResourceIdentifier>
    {
    private:
        string resource_name_;
        unsigned int hash_;
    public:
        ResourceIdentifier()
        {}

        ResourceIdentifier(const string& resource_name)
            : resource_name_(resource_name)
            , hash_(BKDRHash(resource_name.c_str()))
        {
        }

        bool operator < (const ResourceIdentifier& resource_identifier) const
        {
            return hash_ < resource_identifier.hash_ || (hash_ == resource_identifier.hash_ && resource_name_ < resource_identifier.resource_name_);
        }

        string GetResourceName() const { return resource_name_; }

        bool operator == (const ResourceIdentifier& rhs) const
        {
            return hash_ == rhs.hash_ && resource_name_ == rhs.resource_name_;
        }
    };
}

#endif //SUPER_NODE_RESOURCE_IDENTIFIER_H
