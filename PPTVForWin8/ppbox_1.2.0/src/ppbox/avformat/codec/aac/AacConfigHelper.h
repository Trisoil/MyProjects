// AacConfigHelper.h

#ifndef _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_HELPER_H_
#define _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_HELPER_H_

namespace ppbox
{
    namespace avformat
    {

        struct AacConfig;

        class AacConfigHelper
        {
        public:
            AacConfigHelper();

            AacConfigHelper(
                boost::uint8_t const * buf, 
                boost::uint32_t size);

            AacConfigHelper(
                std::vector<boost::uint8_t> const & buf);

            ~AacConfigHelper();

        public:
            AacConfig const & data() const
            {
                return *data_;
            }

        public:
            void from_data(
                std::vector<boost::uint8_t> const & buf);

            void to_data(
                std::vector<boost::uint8_t> & buf) const;

            void from_adts_data(
                std::vector<boost::uint8_t> const & buf);

            void to_adts_data(
                boost::uint32_t frame_size, 
                std::vector<boost::uint8_t> & buf) const;

        public:
            boost::uint32_t get_object_type() const;

            boost::uint32_t get_frequency() const;

            boost::uint32_t get_channel_count() const;

            boost::uint32_t get_extension_object_type() const;

            boost::uint32_t get_extension_frequency() const;

            bool sbr_present() const;

            bool ps_present() const;

        public:
            void set_object_type(
                size_t object_type);

            void set_frequency(
                size_t frequency);

        private:
            AacConfig * data_;

        private:
            static boost::uint32_t frequency_table[];
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_CODEC_AAC_AAC_CONFIG_HELPER_H_
