// AvcSpsPpsType.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_SPS_PPS_TYPE_H_
#define _PPBOX_AVFORMAT_AVC_AVC_SPS_PPS_TYPE_H_

#include "ppbox/avformat/codec/avc/AvcNalu.h"
#include "ppbox/avformat/codec/avc/AvcVuiType.h"

namespace ppbox
{
    namespace avformat
    {

        template <size_t N>
        struct ScalingList
        {
            U<1> present_flag;
            bool use_default_flag;
            boost::int32_t matrix[N];

            SERIALIZATION_SPLIT_MEMBER()

            template <
                typename Archive
            >
            void load(
                Archive & ar)
            {
                ar & present_flag;
                if (present_flag) {
                    SE delta_scale;
                    boost::int32_t last_scale(8), next_scale(8);
                    for (size_t j = 0; j < N; ++j) {
                        if (next_scale != 0) {
                            ar & delta_scale;
                            next_scale = (last_scale + delta_scale + 256) % 256;
                            use_default_flag = (j == 0) && (next_scale == 0);
                        }
                        matrix[j] = (next_scale == 0) ? last_scale : next_scale;
                        last_scale = matrix[j];
                    }
                }
            }

            template <
                typename Archive
            >
            void save(
                Archive & ar) const
            {
                ar & present_flag;
                if (present_flag) {
                    SE delta_scale;
                    size_t n = N - 1;
                    for (; n > 0; --n) {
                        if (matrix[n] != matrix[n - 1])
                            break;
                    }
                    boost::int32_t last_scale(8);
                    for (size_t j = 0; j < N; ++j) {
                        if (j < n) {
                            delta_scale = matrix[j] - last_scale;
                        } else {
                            delta_scale = 256 - last_scale;
                        }
                        ar & delta_scale;
                        last_scale = matrix[j]; 
                    }
                }
            }
        };

        typedef ScalingList<16> ScalingList4x4;
        typedef ScalingList<64> ScalingList8x8;

        struct SeqParameterSetRbsp
            : NaluHeader
        {
            U<8> profile_idc;
            U<1> constraint_set0_flag;
            U<1> constraint_set1_flag;
            U<1> constraint_set2_flag;
            U<5> reserved_zero_5bits;
            U<8> level_idc;
            UE sps_seq_parameter_set_id;
            // if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144) {
                UE chroma_format_idc;
                // if (chroma_format_idc == 3)
                    U<1> residual_colour_transform_flag;
                UE bit_depth_luma_minus8;
                UE bit_depth_chroma_minus8;
                U<1> qpprime_y_zero_transform_bypass_flag;
                U<1> seq_scaling_matrix_present_flag;
                ScalingList4x4 scaling_list_4x4[6];
                ScalingList8x8 scaling_list_8x8[2];
            // }
            UE log2_max_frame_num_minus4;
            UE pic_order_cnt_type;
            // if (pic_order_cnt_type == 0) {
                UE log2_max_pic_order_cnt_lsb_minus4;
            // } else if (pic_order_cnt_type == 1) {
                U<1> delta_pic_order_always_zero_flag;
                SE offset_for_non_ref_pic;
                SE offset_for_top_to_bottom_field;
                UE num_ref_frames_in_pic_order_cnt_cycle;
                // for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
                   std::vector<SE> offset_for_ref_frame;
            // }
            UE num_ref_frames;
            U<1> gaps_in_frame_num_value_allowed_flag;
            UE pic_width_in_mbs_minus1;
            UE pic_height_in_map_units_minus1;
            U<1> frame_mbs_only_flag;
            // if (!frame_mbs_only_flag)
                U<1> mb_adaptive_frame_field_flag;
            U<1> direct_8x8_inference_flag;
            U<1> frame_cropping_flag;
            // if (frame_cropping_flag) {
                UE frame_crop_left_offset;
                UE frame_crop_right_offset;
                UE frame_crop_top_offset;
                UE frame_crop_bottom_offset;
            // }
            U<1> vui_parameters_present_flag;
            // if (vui_parameters_present_flag)
                VuiParameters vui_parameters;
            P rbsp_trailing_bits;

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                NaluHeader::serialize(ar);

                ar & profile_idc
                    & constraint_set0_flag
                    & constraint_set1_flag
                    & constraint_set2_flag
                    & reserved_zero_5bits
                    & level_idc
                    & sps_seq_parameter_set_id;
                if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144) {
                    ar & chroma_format_idc;
                    if (chroma_format_idc == 3) {
                        ar & residual_colour_transform_flag;
                    }
                    ar & bit_depth_luma_minus8
                        & bit_depth_chroma_minus8
                        & qpprime_y_zero_transform_bypass_flag
                        & seq_scaling_matrix_present_flag;
                    if (seq_scaling_matrix_present_flag) {
                        for (size_t i = 0; i < 8; ++i) {
                            if (i < 6) {
                                ar & scaling_list_4x4[i];
                            } else {
                                ar & scaling_list_8x8[i - 6];
                            }
                        }
                    }
                }
                ar & log2_max_frame_num_minus4
                    & pic_order_cnt_type;
                if (pic_order_cnt_type == 0) {
                    ar & log2_max_pic_order_cnt_lsb_minus4;
                } else if (pic_order_cnt_type == 1) {
                    ar & delta_pic_order_always_zero_flag
                        & offset_for_non_ref_pic
                        & offset_for_top_to_bottom_field
                        & num_ref_frames_in_pic_order_cnt_cycle;
                    offset_for_ref_frame.resize(num_ref_frames_in_pic_order_cnt_cycle);
                    for (size_t i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; ++i) {
                        ar & offset_for_ref_frame[i];
                    }
                }
                ar & num_ref_frames
                    & gaps_in_frame_num_value_allowed_flag
                    & pic_width_in_mbs_minus1
                    & pic_height_in_map_units_minus1
                    & frame_mbs_only_flag;
                if (!frame_mbs_only_flag) {
                    ar & mb_adaptive_frame_field_flag;
                }
                ar & direct_8x8_inference_flag
                    & frame_cropping_flag;
                if (frame_cropping_flag) {
                    ar & frame_crop_left_offset
                        & frame_crop_right_offset
                        & frame_crop_top_offset
                        & frame_crop_bottom_offset;
                }
                ar & vui_parameters_present_flag;
                if (vui_parameters_present_flag) {
                    ar & vui_parameters;
                }
                ar & rbsp_trailing_bits; 
            }
       };

       struct PicParameterSetRbsp
           : NaluHeader
       {
       private:
           std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss_;

       public:
           UE pps_pic_parameter_set_id;
           UE pps_seq_parameter_set_id;
           U<1> entropy_coding_mode_flag;
           U<1> pic_order_present_flag;
           UE num_slice_groups_minus1;
           // if (num_slice_groups_minus1 > 0) {
               UE slice_group_map_type;
               // if (slice_group_map_type == 0)
                   //for (iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++)
                       std::vector<UE> run_length_minus1;
               // else if( slice_group_map_type == 2 )
                   // for (iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
                       std::vector<UE> top_left;
                       std::vector<UE> bottom_right;
                   // }
               // else if (slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5) {
                   U<1> slice_group_change_direction_flag;
                   UE slice_group_change_rate_minus1;
               // } else if (slice_group_map_type == 6) {
                   UE pic_size_in_map_units_minus1;
                   // for (i = 0; i <= pic_size_in_map_units_minus1; i++)
                   std::vector<UV<PowerMinus<DirectValue<UE> > > > slice_group_id; // u(v)
               // }
           // }
           UE num_ref_idx_l0_active_minus1;
           UE num_ref_idx_l1_active_minus1;
           U<1> weighted_pred_flag;
           U<2> weighted_bipred_idc;
           SE pic_init_qp_minus26; /* relative to 26 */
           SE pic_init_qs_minus26; /* relative to 26 */
           SE chroma_qp_index_offset;
           U<1> deblocking_filter_control_present_flag;
           U<1> constrained_intra_pred_flag;
           U<1> redundant_pic_cnt_present_flag;
           // if (more_rbsp_data()) {
               U<1> transform_8x8_mode_flag;
               U<1> pic_scaling_matrix_present_flag;
               // if (pic_scaling_matrix_present_flag)
                   // for (i = 0; i < 6 + 2* transform_8x8_mode_flag; i++)
                   ScalingList4x4 scaling_list_4x4[6];
                   ScalingList8x8 scaling_list_8x8[2];
               SE second_chroma_qp_index_offset;
           // }
           P rbsp_trailing_bits;

           PicParameterSetRbsp(
               std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss)
               : spss_(spss)
           {
           }

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               NaluHeader::serialize(ar);

               ar & pps_pic_parameter_set_id
                   & pps_seq_parameter_set_id
                   & entropy_coding_mode_flag
                   & pic_order_present_flag
                   & num_slice_groups_minus1;
               assert(spss_.find(pps_seq_parameter_set_id) != spss_.end());
               if (spss_.find(pps_seq_parameter_set_id) == spss_.end()) {
                   ar.failed();
                   return;
               }
               if (num_slice_groups_minus1 > 0) {
                   ar & slice_group_map_type;
                   if (slice_group_map_type == 0) {
                       run_length_minus1.resize(num_slice_groups_minus1);
                       for (size_t i = 0; i < num_slice_groups_minus1; ++i) {
                           ar & run_length_minus1[i];
                       }
                   } else if( slice_group_map_type == 2 ) {
                       top_left.resize(num_slice_groups_minus1);
                       bottom_right.resize(num_slice_groups_minus1);
                       for (size_t i = 0; i < num_slice_groups_minus1; ++i) {
                           ar & top_left[i]
                               & bottom_right[i];
                       }
                   } else if( slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5) {
                       ar & slice_group_change_direction_flag
                           & slice_group_change_rate_minus1;
                   } else if (slice_group_map_type == 6) {
                       ar & pic_size_in_map_units_minus1;
                       slice_group_id.resize(pic_size_in_map_units_minus1, power_minus(direct_value(num_slice_groups_minus1), 1));
                       for (size_t i = 0; i <pic_size_in_map_units_minus1; ++i) {
                           ar & slice_group_id[i];
                       }
                   }
               }
               ar & num_ref_idx_l0_active_minus1
                   & num_ref_idx_l1_active_minus1 
                   & weighted_pred_flag
                   & weighted_bipred_idc
                   & pic_init_qp_minus26
                   & pic_init_qs_minus26
                   & chroma_qp_index_offset
                   & deblocking_filter_control_present_flag
                   & constrained_intra_pred_flag
                   & redundant_pic_cnt_present_flag;
                if (ar.more()) {
                   ar & transform_8x8_mode_flag
                       & pic_scaling_matrix_present_flag;
                   if (pic_scaling_matrix_present_flag) {
                       for (size_t i = 0; i < 6 + transform_8x8_mode_flag * 2; ++i) {
                           if (i < 6) {
                               ar & scaling_list_4x4[i];
                           } else {
                               ar & scaling_list_8x8[i - 6];
                           }
                       }
                   }
                   ar & second_chroma_qp_index_offset;
               }
               ar & rbsp_trailing_bits; 
           }

           std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss() const
           {
               return spss_;
           }

           SeqParameterSetRbsp const & sps() const
           {
               return (*spss_.find(pps_seq_parameter_set_id)).second;
           }
       };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_SPS_PPS_TYPE_H_
