// Ap4ReadStreamByteStream.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/mp4/Ap4Stream.h"

namespace ppbox
{
    namespace avformat
    {

        AP4_Stream::AP4_Stream(
            std::istream & is, 
            ppbox::data::SegmentInfo const & segment)
            : is_(&is)
            , os_(NULL)
            , ref_(1)
            , segment_(segment)
        {
            offset_ = is.tellg();
            segment_.offset = 0;
        }

        AP4_Stream::AP4_Stream(
            std::ostream & os)
            : is_(NULL)
            , os_(&os)
            , ref_(1)
        {
            offset_ = os.tellp();
        }

        AP4_Stream::~AP4_Stream()
        {
        }

        // methods
        AP4_Result AP4_Stream::ReadPartial(
            void * buffer, 
            AP4_Size  bytes_to_read, 
            AP4_Size & bytes_read)
        {
            is_->read((char*)buffer, bytes_to_read);
            bytes_read = is_->gcount();
            return *is_ ? AP4_SUCCESS : AP4_FAILURE;
        }

        AP4_Result AP4_Stream::WritePartial(
            const void * buffer, 
            AP4_Size bytes_to_write, 
            AP4_Size & bytes_written) 
        {
            os_->write((char*)buffer, bytes_to_write);
            bytes_written = bytes_to_write;
            return *os_ ? AP4_SUCCESS : AP4_FAILURE;
        }

        AP4_Result AP4_Stream::CopyTo(
            AP4_ByteStream & stream, 
            AP4_LargeSize size)
        {
            if (segment_.offset >= segment_.head_size) {
                return AP4_SUCCESS;
            }
            return AP4_ByteStream::CopyTo(stream, size);
        }

        AP4_Result AP4_Stream::Seek(
            AP4_Position position)
        {
            if (is_) {
                // Seek超过有效数据，可以成功，但是读取会失败
                segment_.offset = position;
                if (position > segment_.head_size && position <= segment_.size) {
                    position = segment_.head_size;
                }
                is_->seekg(offset_ + position,std::ios::beg);
            } else {
                os_->seekp(offset_ + position,std::ios::beg);
            }
            return (is_ ? !!*is_ : !!*os_) ? AP4_SUCCESS : AP4_FAILURE;
        }

        AP4_Result AP4_Stream::Tell(
            AP4_Position& position)
        {
            if (is_) {
                if (segment_.offset > segment_.head_size)
                    position = segment_.offset + offset_;
                else
                    position = is_->tellg();
            } else {
                position = os_->tellp();
            }
            position -= offset_;
            return (is_ ? !!*is_ : !!*os_) ? AP4_SUCCESS : AP4_FAILURE;
        }

        AP4_Result AP4_Stream::Flush()
        {
            if (os_) {
                os_->flush();
                return *os_ ? AP4_SUCCESS : AP4_FAILURE;
            }
            return AP4_SUCCESS;
        }

        AP4_Result AP4_Stream::GetSize(
            AP4_LargeSize& size)
        {
            size = segment_.size;
            return AP4_SUCCESS;
        }

        void AP4_Stream::AddReference()
        {
            ++ref_;
        }

        void AP4_Stream::Release()
        {
            if (0 == (--ref_)) {
                delete this;
            }
        }

    } // namespace avformat
} // namespace ppbox
