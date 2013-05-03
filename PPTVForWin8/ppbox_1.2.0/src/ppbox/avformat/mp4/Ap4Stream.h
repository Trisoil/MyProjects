// AP4_ReadStreamByteStream.h

#ifndef _PPBOX_MERGE_AP4_STREAM_BYTE_STREAM_H_
#define _PPBOX_MERGE_AP4_STREAM_BYTE_STREAM_H_

#include <ppbox/data/segment/SegmentInfo.h>

#include <bento4/Core/Ap4ByteStream.h>

#include <iostream>

namespace ppbox
{
    namespace avformat
    {

        class AP4_Stream
            : public AP4_ByteStream
        {
        public:
            AP4_Stream(
                std::istream & is, 
                ppbox::data::SegmentInfo const & segment);

            AP4_Stream(
                std::ostream & os);

            virtual ~AP4_Stream();

        public:
            virtual AP4_Result ReadPartial(
                void * buffer, 
                AP4_Size bytes_to_read, 
                AP4_Size & bytes_read);

            virtual AP4_Result WritePartial(
                void const * buffer, 
                AP4_Size bytes_to_write, 
                AP4_Size & bytes_written) ;;

            virtual AP4_Result CopyTo(
                AP4_ByteStream& stream, 
                AP4_LargeSize size);

            virtual AP4_Result Seek(
                AP4_Position position);

            virtual AP4_Result Tell(
                AP4_Position& position);

            virtual AP4_Result GetSize(
                AP4_LargeSize& size);

            virtual AP4_Result Flush();

        public:
            virtual void AddReference();

            virtual void Release();

        private:
            std::istream * is_;
            std::ostream * os_;
            AP4_Size ref_;
            AP4_Size offset_;
            ppbox::data::SegmentInfo segment_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_MERGE_AP4_STREAM_BYTE_STREAM_H_
