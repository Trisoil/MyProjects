// AacType.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/codec/aac/AacCodec.h"

namespace ppbox
{
    namespace avformat
    {

        AacCodec::AacCodec()
        {
        }

        AacCodec::AacCodec(
            std::vector<boost::uint8_t> const & config)
            : config_helper_(config)
        {
        }

        AacCodec::AacCodec(
            std::vector<boost::uint8_t> const & config, 
            from_adts_tag)
        {
            config_helper_.from_adts_data(config);
        }

    } // namespace avformat
} // namespace ppbox
