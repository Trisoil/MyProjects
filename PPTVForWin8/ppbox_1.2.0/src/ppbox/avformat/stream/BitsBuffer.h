// BitsBuffer.h

#ifndef _PPBOX_AVFORMAT_STREAM_BITS_BUFFER_H_
#define _PPBOX_AVFORMAT_STREAM_BITS_BUFFER_H_

#include <streambuf>

namespace ppbox
{
    namespace avformat
    {

        template <
            typename _Elem = char, 
            typename _Traits = std::char_traits<_Elem>
        >
        class BitsBuffer
            : public std::basic_streambuf<_Elem, _Traits>
        {
        public:
            typedef typename std::basic_streambuf<_Elem, _Traits>::char_type char_type;
            typedef typename std::basic_streambuf<_Elem, _Traits>::int_type int_type;
            typedef typename std::basic_streambuf<_Elem, _Traits>::traits_type traits_type;

            BitsBuffer(
                std::basic_streambuf<_Elem, _Traits> & next_layer)
                : next_layer_(next_layer)
                , byte_in_(0)
                , byte_out_(0)
                , num_zero_in_(0)
                , num_zero_out_(0)
            {
                this->setg(NULL, NULL, NULL);
                this->setp(NULL, NULL);
            }

        private:
            virtual int_type underflow()
            {
                int_type c = next_layer_.sbumpc();
                if (num_zero_in_ == 2 && c == traits_type::to_int_type(3)) {
                    num_zero_in_ = 0;
                    c = next_layer_.sbumpc();
                }
                if (c == traits_type::to_int_type(0)) {
                    ++num_zero_in_;
                } else {
                    num_zero_in_ = 0;
                }
                if (c != traits_type::eof()) {
                    byte_in_ = traits_type::to_char_type(c);
                    this->setg(&byte_in_, &byte_in_, &byte_in_ + 1);
                }
                return c;
            }

            virtual int_type overflow(int_type c)
            {
                if (num_zero_out_ == 2 && 0 == (traits_type::to_char_type(c) >> 2)) {
                    num_zero_out_ = 0;
                    int_type three = next_layer_.sputc(traits_type::to_char_type(3));
                    if (three == traits_type::eof())
                        return three;
                }
                if (c == traits_type::to_int_type(0)) {
                    ++num_zero_out_;
                } else {
                    num_zero_out_ = 0;
                }
                return next_layer_.sputc(traits_type::to_char_type(c));
            }

        private:
            std::basic_streambuf<_Elem, _Traits> & next_layer_;
            _Elem byte_in_;
            _Elem byte_out_;
            size_t num_zero_in_;
            size_t num_zero_out_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_STREAM_BITS_BUFFER_H_
