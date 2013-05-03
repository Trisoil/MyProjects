// Codec.h

#ifndef _PPBOX_AVFORMAT_CODEC_CODEC_H_
#define _PPBOX_AVFORMAT_CODEC_CODEC_H_

#include <ppbox/common/ClassFactory.h>

#include <boost/intrusive_ptr.hpp>

namespace ppbox
{
    namespace avformat
    {

        class Codec
            : public ppbox::common::ClassFactory<
                Codec, 
                boost::uint32_t, 
                Codec *(std::vector<boost::uint8_t> const &)
            >
        {
        public:
            Codec()
            {
            }

            virtual ~Codec()
            {
            }

        private:
            friend void intrusive_ptr_add_ref(
                Codec const * p)
            {
                ++p->nref_;
            }

            friend void intrusive_ptr_release(
                Codec const * p)
            {
                if (--p->nref_ == 0) {
                    delete p;
                }
            }

        private:
            mutable size_t nref_;
        };

    } // namespace avformat
} // namespace ppbox

#define PPBOX_REGISTER_CODEC(key, cls) PPBOX_REGISTER_CLASS(key, cls)

#endif // _PPBOX_AVFORMAT_CODEC_CODEC_H_
