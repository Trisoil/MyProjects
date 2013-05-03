// SampleBuffers.h

#ifndef _PPBOX_AVFORMAT_SAMPLE_BUFFERS_H_
#define _PPBOX_AVFORMAT_SAMPLE_BUFFERS_H_

#include <util/buffers/BuffersFind.h>

namespace ppbox
{
    namespace avformat
    {

        struct SampleBuffers
        {
            typedef std::deque<boost::asio::const_buffer> ConstBuffers;

            typedef util::buffers::BuffersPosition<
                ConstBuffers::value_type, 
                ConstBuffers::const_iterator
            > BuffersPosition;

            typedef util::buffers::BuffersFindIterator2<
                ConstBuffers, 
                boost::asio::const_buffers_1
            > FindIterator2;

            typedef FindIterator2::ByteIterator ByteIterator;

            typedef FindIterator2::BuffersIterator BufferIterator;

            typedef util::buffers::Container<boost::asio::const_buffer, BufferIterator> RangeBuffers;

            typedef util::buffers::Container<BuffersPosition::Byte, ByteIterator> RangeBytes;

            static BufferIterator range_buffers_begin(
                BuffersPosition const & beg, 
                BuffersPosition const & end)
            {
                return BufferIterator(beg, end);
            }

            static BufferIterator range_buffers_end()
            {
                return BufferIterator();
            }

            static ByteIterator range_bytes_begin(
                BuffersPosition const & beg, 
                BuffersPosition const & end)
            {
                return ByteIterator(beg, end);
            }

            static ByteIterator range_bytes_end()
            {
                return ByteIterator();
            }

            static RangeBuffers range_buffers(
                BuffersPosition const & beg, 
                BuffersPosition const & end)
            {
                return RangeBuffers(BufferIterator(beg, end));
            }

            static RangeBytes range_bytes(
                BuffersPosition const & beg, 
                BuffersPosition const & end)
            {
                return RangeBytes(ByteIterator(beg, end));
            }
       };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_SAMPLE_BUFFERS_H_
