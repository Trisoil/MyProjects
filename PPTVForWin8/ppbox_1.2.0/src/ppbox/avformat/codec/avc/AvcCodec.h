// AvcCodec.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_CODEC_H_
#define _PPBOX_AVFORMAT_AVC_AVC_CODEC_H_

#include "ppbox/avformat/codec/Codec.h"
#include "ppbox/avformat/codec/avc/AvcConfigHelper.h"
#include "ppbox/avformat/Format.h"

namespace ppbox
{
    namespace avformat
    {

        class AvcCodec
            : public Codec
        {
        public:
            AvcCodec();

            AvcCodec(
                std::vector<boost::uint8_t> const & config);

            struct from_es_tag {};

            AvcCodec(
                std::vector<boost::uint8_t> const & config, 
                from_es_tag);

        public:
            AvcConfigHelper const & config_helper() const
            {
                return config_helper_;
            }

            AvcConfig const & config() const
            {
                return config_helper_.data();
            }

        private:
            AvcConfigHelper config_helper_;
        };

        PPBOX_REGISTER_CODEC(VIDEO_TYPE_AVC1, AvcCodec);

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_CODEC_H_
