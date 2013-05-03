// PacketBuffer.h

#ifndef _MUTEX_CLIENT_PACKET_BUFFER_H_
#define _MUTEX_CLIENT_PACKET_BUFFER_H_

#include <boost/asio/buffer.hpp>

#include <streambuf>
#include <stdexcept>

namespace mutex
{
    namespace client
    {

        template <
            typename _Elem = char, 
            typename _Traits = std::char_traits<_Elem>
        >
        class PacketBuffer
            : public std::basic_streambuf<_Elem, _Traits>
        {
        public:
            typedef typename std::basic_streambuf<_Elem, _Traits>::char_type char_type;
            typedef typename std::basic_streambuf<_Elem, _Traits>::int_type int_type;
            typedef typename std::basic_streambuf<_Elem, _Traits>::traits_type traits_type;

            std::size_t size()
            {
                return this->pptr() - this->gptr();
            }

            PacketBuffer()
            {
                this->setg(buf_, buf_, buf_);
                this->setp(buf_, buf_ + buf_size_);
            }

            /**
            ���ض����������

            �����ƶ���ָ�롣
            */
            boost::asio::const_buffers_1 data(std::size_t n = 1024)
            {
                if (this->gptr() + n > this->pptr())
                    n = this->pptr() - this->gptr();
                return boost::asio::const_buffers_1(this->gptr(), n);
            }

            /**
            ��д����������ύ�Ķ�����

            ׷�Ӵ�дָ�뿪ʼ�� @c n �ֽ����ݵ������棬дָ����ǰ�ƶ� @c n �ֽڡ�
            ��Ҫ���ⲿд��ʵ�����ݣ�Ȼ���ύ
            */
            void commit(std::size_t n)
            {
                if (this->pptr() + n > this->epptr())
                    n = this->epptr() - this->pptr();
                this->pbump(static_cast<int>(n));
                this->setg(this->eback(), this->gptr(), this->pptr());
            }

            /**
            �Ӷ������Ƴ�����

            ��ָ����ǰ�ƶ� @c n �ֽڡ�
            ��Ҫ���ⲿ����ʵ�����ݣ�Ȼ���Ƴ���Ӧ����
            */
            void consume(std::size_t n)
            {
                //TODO: �Ż�
                /*if (this->gptr() + n > this->pptr())
                n = this->pptr() - this->gptr();
                this->gbump(static_cast<int>(n));
                this->setg(this->eback(), this->gptr(), this->pptr());*/
                // ��ʱ���
                this->setg(buf_, buf_, buf_);
                this->setp(buf_, buf_ + buf_size_);
            }

            /**
            ��д������׼��һ�οռ�

            �����ƶ�дָ�롣
            */
            boost::asio::mutable_buffers_1 prepare(std::size_t n)
            {
                if (this->pptr() + n > this->epptr())
                    n = this->epptr() - this->pptr();
                return boost::asio::mutable_buffers_1(this->pptr(), n);
            }

            void reset()
            {
                this->setg(buf_, buf_, buf_);
                this->setp(buf_, buf_ + buf_size_);
            }

        protected:
            virtual int_type underflow()
            {
                return traits_type::eof();
            }

            virtual int_type overflow(int_type c)
            {
                throw std::length_error("archive buffer too long");
                return traits_type::not_eof(c);
            }

        private:
            char_type buf_[1024]; ///< �����׵�ַ
            static std::size_t const buf_size_ = 1024; ///< �����С
        }; 

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_PACKET_BUFFER_H_
