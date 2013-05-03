// LogBuffer.h

#include <framework/logger/Stream.h>

#include <boost/asio/buffer.hpp>

namespace boost
{
    namespace asio
    {

        const_buffers_1 buffer(framework::logger::Stream::buffer_t const & data)
        {
            return const_buffers_1(const_buffer(data.buf, data.len));
        }

    }
}
