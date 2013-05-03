//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_COMPONENT_STATUS_ITEM_H
#define SUPER_NODE_COMPONENT_STATUS_ITEM_H

#include "StatusItemConstraint.h"

namespace super_node
{
    class ComponentStatusItem
        :public count_object_allocate<ComponentStatusItem>
    {
    public:
        ComponentStatusItem()
            : value_(0), set_value_(false)
        {}

        ComponentStatusItem(const string& item_name, const string& unit, boost::shared_ptr<StatusItemConstraint> constraint)
            : item_name_(item_name), unit_(unit), constraint_(constraint), value_(0), set_value_(false)
        {
        }

        const string& GetItemName() const
        {
            return item_name_;
        }

        bool IsHealthy() const
        {
            if (!constraint_ || !set_value_)
            {
                return true;
            }

            return constraint_->Healthy(value_);
        }

        void SetStatus(size_t value)
        {
            value_ = value;
            set_value_ = true;;
        }

        size_t GetStatus() const 
        {
            return value_;
        }

        string GetStatusDescription() const
        {
            std::ostringstream status;
            status<<item_name_<<": "<<value_<<" "<<unit_;
            return status.str();
        }

    private:
        string item_name_;
        string unit_;
        size_t value_;
        bool set_value_;
        boost::shared_ptr<StatusItemConstraint> constraint_;
    };
}

#endif //SUPER_NODE_COMPONENT_STATUS_ITEM_H