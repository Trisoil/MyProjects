//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_SERVICE_COMPONENT_H
#define SUPER_NODE_SERVICE_COMPONENT_H

#include "ComponentStatusItem.h"
#include <boost/thread/mutex.hpp>

namespace super_node
{
    class StatusItemConstraint;

    class IServiceComponentHealthPredicate
    {
    public:
        virtual bool IsComponentHealthy() const = 0;
        virtual ~IServiceComponentHealthPredicate(){}
    };

    class ServiceComponent
    {
    public:
        ServiceComponent(const string& component_name)
            : component_name_(component_name)
        {
        }

        void GetStatusItems(std::map<string, boost::shared_ptr<ComponentStatusItem> >& status_items)
        {
            boost::mutex::scoped_lock lock(mutex_);
            status_items = status_items_;
        }

        boost::shared_ptr<ComponentStatusItem> RegisterStatusItem(const string& item_name, const string& unit, boost::shared_ptr<StatusItemConstraint> constraint = boost::shared_ptr<StatusItemConstraint>())
        {
            boost::mutex::scoped_lock lock(mutex_);
            assert(status_items_.find(item_name) == status_items_.end());
            status_items_[item_name] = boost::shared_ptr<ComponentStatusItem>(new ComponentStatusItem(item_name, unit, constraint));
            return status_items_[item_name];
        }

        const string& GetComponentName() const
        {
            return component_name_;
        }

        bool IsHealthy() const
        {
            return health_predicate_ ? health_predicate_->IsComponentHealthy() : true;
        }

        void SetHealthPredicate(boost::shared_ptr<IServiceComponentHealthPredicate> health_predicate)
        {
            health_predicate_ = health_predicate;
        }

        void Unregister()
        {
            if (health_predicate_)
            {
                health_predicate_.reset();
            }
        }

    private:
        boost::mutex mutex_;
        string component_name_;
        std::map<string, boost::shared_ptr<ComponentStatusItem> > status_items_;
        boost::shared_ptr<IServiceComponentHealthPredicate> health_predicate_;
    };
}

#endif //SUPER_NODE_SERVICE_COMPONENT_H