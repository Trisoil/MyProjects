// ProtocolHandle.cpp

#include "mutex/client/Common.h"
#include "mutex/client/ProtocolHandle.h"

namespace mutex
{
    namespace client
    {

        ProtocolHandle::ProtocolHandle(void)
            : is_running_(true)
            , sequece_(rand())
        {
            sequece_ |= 1; // ±ØÐëÊÇÆæÊý
        }

        ProtocolHandle::~ProtocolHandle(void)
        {
        }

    } // namespace client
} // namespace mutex
