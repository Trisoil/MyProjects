// AvcVuiType.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_VUI_TYPE_H_
#define _PPBOX_AVFORMAT_AVC_AVC_VUI_TYPE_H_

namespace ppbox
{
    namespace avformat
    {

        struct HrdParameters
        {
            UE cpb_cnt_minus1;
            U<4> bit_rate_scale;
            U<4> cpb_size_scale;
            // for( SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {
                std::vector<UE> bit_rate_value_minus1;
                std::vector<UE> cpb_size_value_minus1;
                std::vector<U<1> > cbr_flag;
            // }
            U<5> initial_cpb_removal_delay_length_minus1;
            U<5> cpb_removal_delay_length_minus1;
            U<5> dpb_output_delay_length_minus1;
            U<5> time_offset_length;

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & cpb_cnt_minus1
                    & bit_rate_scale
                    & cpb_size_scale;
                bit_rate_value_minus1.resize(cpb_cnt_minus1);
                cpb_size_value_minus1.resize(cpb_cnt_minus1);
                cbr_flag.resize(cpb_cnt_minus1);
                for (size_t i = 0; i < cpb_cnt_minus1; ++i) {
                    ar & bit_rate_value_minus1[i]
                        & cpb_size_value_minus1[i]
                        & cbr_flag[i];
                }
                ar & initial_cpb_removal_delay_length_minus1
                    & cpb_removal_delay_length_minus1
                    & dpb_output_delay_length_minus1
                    & time_offset_length;
            }
        };

        struct VuiParameters
        {
            U<1> aspect_ratio_info_present_flag;
            // if (aspect_ratio_info_present_flag) {
                U<8> aspect_ratio_idc;
                // if (aspect_ratio_idc == Extended_SAR) {
                    U<16> sar_width;
                    U<16> sar_height;
                // }
            // }
            U<1> overscan_info_present_flag;
            // if (overscan_info_present_flag)
                U<1> overscan_appropriate_flag;
            U<1> video_signal_type_present_flag;
            // if (video_signal_type_present_flag) {
                U<3> video_format;
                U<1> video_full_range_flag;
                U<1> colour_description_present_flag;
                // if (colour_description_present_flag) {
                   U<8> colour_primaries;
                   U<8> transfer_characteristics;
                   U<8> matrix_coefficients;
                // }
            // }
            U<1> chroma_loc_info_present_flag;
            // if (chroma_loc_info_present_flag) {
                UE chroma_sample_loc_type_top_field;
                UE chroma_sample_loc_type_bottom_field;
            // }
            U<1> timing_info_present_flag;
            // if (timing_info_present_flag) {
                U<32> num_units_in_tick;
                U<32> time_scale;
                U<1> fixed_frame_rate_flag;
            // }
            U<1> nal_hrd_parameters_present_flag;
            // if (nal_hrd_parameters_present_flag)
                HrdParameters nal_hrd_parameters;
            U<1> vcl_hrd_parameters_present_flag;
            // if (vcl_hrd_parameters_present_flag)
                HrdParameters vcl_hrd_parameters;
            // if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
                U<1> low_delay_hrd_flag;
            U<1> pic_struct_present_flag;
            U<1> bitstream_restriction_flag;
            // if (bitstream_restriction_flag) {
                U<1> motion_vectors_over_pic_boundaries_flag;
                UE max_bytes_per_pic_denom;
                UE max_bits_per_mb_denom;
                UE log2_max_mv_length_horizontal;
                UE log2_max_mv_length_vertical;
                UE num_reorder_frames;
                UE max_dec_frame_buffering;
            // }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & aspect_ratio_info_present_flag;
                if (aspect_ratio_info_present_flag) {
                    ar & aspect_ratio_idc;
                    if (aspect_ratio_idc == 255) {
                        ar & sar_width
                            & sar_height;
                    }
                }
                ar & overscan_info_present_flag;
                if (overscan_info_present_flag) {
                    ar & overscan_appropriate_flag;
                }
                ar & video_signal_type_present_flag;
                if (video_signal_type_present_flag) {
                    ar & video_format
                        & video_full_range_flag
                        & colour_description_present_flag;
                    if (colour_description_present_flag) {
                        ar & colour_primaries
                            & transfer_characteristics
                            & matrix_coefficients;
                    }
                }
                ar & chroma_loc_info_present_flag;
                if (chroma_loc_info_present_flag) {
                    ar & chroma_sample_loc_type_top_field
                        & chroma_sample_loc_type_bottom_field;
                }
                ar & timing_info_present_flag;
                if (timing_info_present_flag) {
                    ar & num_units_in_tick
                        & time_scale
                        & fixed_frame_rate_flag;
                }
                ar & nal_hrd_parameters_present_flag;
                if (nal_hrd_parameters_present_flag) {
                    ar & nal_hrd_parameters;
                }
                ar & vcl_hrd_parameters_present_flag;
                if (vcl_hrd_parameters_present_flag) {
                    ar & vcl_hrd_parameters;
                }
                if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
                    ar & low_delay_hrd_flag;
                }
                ar & pic_struct_present_flag
                    & bitstream_restriction_flag;
                if (bitstream_restriction_flag) {
                    ar & motion_vectors_over_pic_boundaries_flag
                        & max_bytes_per_pic_denom
                        & max_bits_per_mb_denom
                        & log2_max_mv_length_horizontal
                        & log2_max_mv_length_vertical
                        & num_reorder_frames
                        & max_dec_frame_buffering;
                }
            }
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_VUI_TYPE_H_
