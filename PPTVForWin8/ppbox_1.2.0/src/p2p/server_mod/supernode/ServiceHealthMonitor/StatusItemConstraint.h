//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_STATUS_ITEM_CONSTRAINT_H
#define SUPER_NODE_STATUS_ITEM_CONSTRAINT_H

namespace super_node
{
    class StatusItemConstraint
    {
    public:
        virtual bool Healthy(size_t value) const = 0;
    };

    class MinimumValueConstraint
        : public StatusItemConstraint
    {
    public:
        MinimumValueConstraint(size_t minimum)
            : minimum_(minimum)
        {}

        bool Healthy(size_t value) const
        {
            return value >= minimum_;
        }

    private:
        size_t minimum_;
    };

    class MaximumValueConstraint
        : public StatusItemConstraint
    {
    public:
        MaximumValueConstraint(size_t maximum)
            : maximum_(maximum)
        {}

        bool Healthy(size_t value) const
        {
            return value <= maximum_;
        }

    private:
        size_t maximum_;
    };
}

#endif //SUPER_NODE_STATUS_ITEM_CONSTRAINT_H