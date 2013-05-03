// AvcType.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/codec/avc/AvcCodec.h"

namespace ppbox
{
    namespace avformat
    {

        AvcCodec::AvcCodec()
        {
        }

        AvcCodec::AvcCodec(
            std::vector<boost::uint8_t> const & config)
            : config_helper_(config)
        {
        }

        AvcCodec::AvcCodec(
            std::vector<boost::uint8_t> const & config, 
            from_es_tag)
        {
            config_helper_.from_es_data(config);
        }

    } // namespace avformat
} // namespace ppbox
