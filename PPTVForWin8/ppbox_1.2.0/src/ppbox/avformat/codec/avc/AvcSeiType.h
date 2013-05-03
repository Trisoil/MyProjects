// AvcSeiType.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_SEI_TYPE_H_
#define _PPBOX_AVFORMAT_AVC_AVC_SEI_TYPE_H_

#include "ppbox/avformat/codec/avc/AvcNalu.h"
#include "ppbox/avformat/codec/avc/AvcSpsPpsType.h"

namespace ppbox
{
    namespace avformat
    {

        struct SeiContext
        {
            SeiContext(
                std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss, 
                std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
                : spss(spss)
                , ppss(ppss)
                , seq_parameter_set_id(0)
                , payload_size(0)
            {
            }

            std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss;
            std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss;
            boost::uint32_t seq_parameter_set_id;

            boost::uint32_t payload_size;

            SeqParameterSetRbsp const & sps() const
            {
                return (*spss.find(seq_parameter_set_id)).second;
            }
        
            VuiParameters const & vui_parameters() const
            {
                return sps().vui_parameters;
            }
        };

        struct BufferingPeriod
        {
        private:
            SeiContext & context_;
        
        public:
            static int const type = 0;

            typedef UV<Minus<DirectValue<U<5> > > > MyUV;
        
            UE seq_parameter_set_id;
            // if (NalHrdBpPresentFlag) {
                // for (SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++) {
                    std::vector<MyUV> nal_initial_cpb_removal_delay;
                    std::vector<MyUV> nal_initial_cpb_removal_delay_offset;
                // }
            // }
            // if (VclHrdBpPresentFlag) {
                // for( SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {
                    std::vector<MyUV> vcl_initial_cpb_removal_delay;
                    std::vector<MyUV> vcl_initial_cpb_removal_delay_offset;
                // }
            // }
        
            BufferingPeriod(
                SeiContext & context)
                : context_(context)
            {
            }
        
            VuiParameters const & vui_parameters() const
            {
                return context_.vui_parameters();
            }
        
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                boost::uint32_t bits_left = context_.payload_size * 8;
                ar & seq_parameter_set_id;
                bits_left -= seq_parameter_set_id.format_size();
                if (vui_parameters().nal_hrd_parameters_present_flag || bits_left > 8) {
                    U<5> const & initial_cpb_removal_delay_length_minus1 = 
                        vui_parameters().nal_hrd_parameters.initial_cpb_removal_delay_length_minus1;
                    nal_initial_cpb_removal_delay.resize(vui_parameters().nal_hrd_parameters.cpb_cnt_minus1 + 1, 
                        minus(direct_value(initial_cpb_removal_delay_length_minus1), 1));
                    nal_initial_cpb_removal_delay_offset.resize(vui_parameters().nal_hrd_parameters.cpb_cnt_minus1 + 1, 
                        minus(direct_value(initial_cpb_removal_delay_length_minus1), 1));
                    for (boost::uint32_t i = 0; i <= vui_parameters().nal_hrd_parameters.cpb_cnt_minus1; ++i) {
                        ar & nal_initial_cpb_removal_delay[i];
                        ar & nal_initial_cpb_removal_delay_offset[i];
                    }
                    bits_left -= minus(direct_value(initial_cpb_removal_delay_length_minus1), 1)() * (vui_parameters().nal_hrd_parameters.cpb_cnt_minus1 + 1);
                }
                if (vui_parameters().vcl_hrd_parameters_present_flag || bits_left > 8) {
                    U<5> const & initial_cpb_removal_delay_length_minus1 = 
                        vui_parameters().vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1;
                    vcl_initial_cpb_removal_delay.resize(vui_parameters().vcl_hrd_parameters.cpb_cnt_minus1 + 1, 
                        minus(direct_value(initial_cpb_removal_delay_length_minus1), 1));
                    vcl_initial_cpb_removal_delay_offset.resize(vui_parameters().vcl_hrd_parameters.cpb_cnt_minus1 + 1, 
                        minus(direct_value(initial_cpb_removal_delay_length_minus1), 1));
                    for (boost::uint32_t i = 0; i <= vui_parameters().vcl_hrd_parameters.cpb_cnt_minus1; ++i) {
                        ar & vcl_initial_cpb_removal_delay[i];
                        ar & vcl_initial_cpb_removal_delay_offset[i];
                    }
                    bits_left -= minus(direct_value(initial_cpb_removal_delay_length_minus1), 1)() * (vui_parameters().nal_hrd_parameters.cpb_cnt_minus1 + 1);
                }
                std::cout << "BufferingPeriod bits_left: " << bits_left << std::endl;
                U<8> u8;
                while (bits_left >= 8) {
                    ar & u8;
                    bits_left -= 8;
                }
                std::cout << "BufferingPeriod bits_left: " << bits_left << std::endl;
            }
        };

        struct PicTiming
        {
        private:
            SeiContext const & context_;
        
        public:
            static int const type = 1;

            struct clock_timestamp
            {
                clock_timestamp(
                    U<5> const & time_offset_length)
                    : time_offset(direct_value(time_offset_length))
                {
                }
        
                U<2> ct_type;
                U<1> nuit_field_based_flag;
                U<5> counting_type;
                U<1> full_timestamp_flag;
                U<1> discontinuity_flag;
                U<1> cnt_dropped_flag;
                U<8> n_frames;
                // if (full_timestamp_flag) {
                   // U<6> seconds_value; /* 0..59 */
                   // U<6> minutes_value; /* 0..59 */
                   // U<5> hours_value; /* 0..23 */
                // } else {
                    U<1> seconds_flag;
                    // if (seconds_flag) {
                        U<6> seconds_value; /* range 0..59 */
                        U<1> minutes_flag;
                        // if (minutes_flag) {
                            U<6> minutes_value; /* 0..59 */
                            U<1> hours_flag;
                            // if (hours_flag)
                                U<5> hours_value; /* 0..23 */
                        // }
                    // }
                // }
                // if (time_offset_length > 0)
                    IV<DirectValue<U<5> > > time_offset;
        
                boost::uint32_t bit_size() const
                {
                    return 19 + (full_timestamp_flag ? 17 
                        : (1 + (seconds_flag ? (7 + (minutes_flag ? (7 + (hours_flag ? 5 : 0)) : 0)) : 0))) 
                        + time_offset();
                }

                template <
                    typename Archive
                >
                void serialize(
                Archive & ar)
                {
                    ar & ct_type
                        & nuit_field_based_flag
                        & counting_type
                        & full_timestamp_flag
                        & discontinuity_flag
                        & cnt_dropped_flag
                        & n_frames;
                    if (full_timestamp_flag) {
                        ar & seconds_value
                            & minutes_value
                            & hours_value;
                    } else {
                        ar & seconds_flag;
                        if (seconds_flag) {
                            ar & seconds_value
                                & minutes_flag;
                            if (minutes_flag) {
                                ar & minutes_value
                                    & hours_flag;
                                if (hours_flag)
                                    ar & hours_value;
                            }
                        }
                    }
                    if (time_offset() > 0)
                        ar & time_offset;
                }
            };
        
            // if (CpbDpbDelaysPresentFlag) {
                UV<Minus<ConstMemFunc<PicTiming, U<5> const &> > > cpb_removal_delay;
                UV<Minus<ConstMemFunc<PicTiming, U<5> const &> > > dpb_output_delay;
            // }
            // if (pic_struct_present_flag) {
                U<4> pic_struct;
                // for (i = 0; i < NumClockTS ; i++) {
                    std::vector<U<1> > clock_timestamp_flag;
                    std::vector<clock_timestamp> clock_timestamps;
                // }
            // }
        
            PicTiming(
                SeiContext & context)
                : context_(context)
                , cpb_removal_delay(minus(mem_func(this, &PicTiming::cpb_removal_delay_length_minus1), 1))
                , dpb_output_delay(minus(mem_func(this, &PicTiming::dpb_output_delay_length_minus1), 1))
            {
            }
        
            VuiParameters const & vui_parameters() const
            {
                return context_.vui_parameters();
            }
        
            U<5> const & cpb_removal_delay_length_minus1() const
            {
                return vui_parameters().nal_hrd_parameters_present_flag 
                    ? vui_parameters().nal_hrd_parameters.cpb_removal_delay_length_minus1 
                    : vui_parameters().vcl_hrd_parameters.cpb_removal_delay_length_minus1;
            }
        
            U<5> const & dpb_output_delay_length_minus1() const
            {
                return vui_parameters().nal_hrd_parameters_present_flag 
                    ? vui_parameters().nal_hrd_parameters.dpb_output_delay_length_minus1 
                    : vui_parameters().vcl_hrd_parameters.dpb_output_delay_length_minus1;
            }
        
            U<5> const & time_offset_length()
            {
                return vui_parameters().nal_hrd_parameters_present_flag 
                    ? vui_parameters().nal_hrd_parameters.time_offset_length 
                    : vui_parameters().vcl_hrd_parameters.time_offset_length;
            }
        
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                boost::uint32_t bits_left = context_.payload_size * 8;
                if (vui_parameters().nal_hrd_parameters_present_flag || 
                    vui_parameters().vcl_hrd_parameters_present_flag) {
                        ar & cpb_removal_delay
                            & dpb_output_delay;
                        bits_left -= cpb_removal_delay();
                        bits_left -= dpb_output_delay();
                }
                if (vui_parameters().pic_struct_present_flag) {
                    ar & pic_struct;
                    bits_left -= 4;
                    boost::uint8_t const num_clock_ts_tbl[] = {1, 1, 1, 2, 2, 3, 3, 2, 3};
                    boost::uint8_t const num_clock_ts = num_clock_ts_tbl[pic_struct];
                    clock_timestamp_flag.resize(num_clock_ts);
                    clock_timestamps.resize(num_clock_ts, clock_timestamp(time_offset_length()));
                    for (boost::uint8_t i = 0; i < num_clock_ts ; i++) {
                        ar & clock_timestamp_flag[i];
                        bits_left -= 1;
                        if (clock_timestamp_flag[i]) {
                            ar & clock_timestamps[i];
                            bits_left -= clock_timestamps[i].bit_size();
                        }
                    }
                }
                std::cout << "PicTiming bits_left: " << bits_left << std::endl;
                U<8> u8;
                while (bits_left >= 8) {
                    ar & u8;
                    bits_left -= 8;
                }
                std::cout << "PicTiming bits_left: " << bits_left << std::endl;
            }
        };

        struct PanScanRect
        {
            static int const type = 2;

            UE pan_scan_rect_id;
            U<1> pan_scan_rect_cancel_flag;
            // if( !pan_scan_rect_cancel_flag ) {
                UE pan_scan_cnt_minus1;
                // for (i = 0; i <= pan_scan_cnt_minus1; i++) {
                    std::vector<SE> pan_scan_rect_left_offset;
                    std::vector<SE> pan_scan_rect_right_offset;
                    std::vector<SE> pan_scan_rect_top_offset;
                    std::vector<SE> pan_scan_rect_bottom_offset;
                // }
                UE pan_scan_rect_repetition_period;
            // }

            PanScanRect(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & pan_scan_rect_id
                    & pan_scan_rect_cancel_flag;
                if (!pan_scan_rect_cancel_flag) {
                    ar & pan_scan_cnt_minus1;
                    pan_scan_rect_left_offset.resize(pan_scan_cnt_minus1 + 1);
                    pan_scan_rect_right_offset.resize(pan_scan_cnt_minus1 + 1);
                    pan_scan_rect_top_offset.resize(pan_scan_cnt_minus1 + 1);
                    pan_scan_rect_bottom_offset.resize(pan_scan_cnt_minus1 + 1);
                    for (boost::uint32_t i = 0; i <= pan_scan_cnt_minus1; i++) {
                         ar & pan_scan_rect_left_offset[i]
                             & pan_scan_rect_right_offset[i]
                             & pan_scan_rect_top_offset[i]
                             & pan_scan_rect_bottom_offset[i];
                    }
                    ar & pan_scan_rect_repetition_period;
                }
            }
        };

        struct FillerPayload
        {
        private:
            SeiContext const & context_;

        public:
            static int const type = 3;

            // for( k = 0; k < payload_size; k++)
                U<8> ff_byte; /* equal to 0xFF */

            FillerPayload(
                SeiContext & context)
                : context_(context)
                , ff_byte(255)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                for (boost::uint32_t i = 0; i < context_.payload_size; ++i) {
                    ar & ff_byte;
                } 
            }
        };

        struct UserDataRegisteredItuTT35
        {
        private:
            SeiContext const & context_;

        public:
            static int const type = 4;

            U<8> itu_t_t35_country_code;
            // if( itu_t_t35_country_code != 0xFF )
                // i = 1
            // else {
                U<8> itu_t_t35_country_code_extension_byte;
                // i = 2
            //}
            // do {
                std::vector<U<8> > itu_t_t35_payload_byte;
                // i++
            // } while( i < payloadSize )

            UserDataRegisteredItuTT35(
                SeiContext & context)
                : context_(context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                boost::uint32_t n = context_.payload_size - 1;
                ar & itu_t_t35_country_code;
                if (itu_t_t35_country_code == 0xff) {
                    ar & itu_t_t35_country_code_extension_byte;
                    --n;
                }
                itu_t_t35_payload_byte.resize(n);
                for (boost::uint32_t i = 0; i < n; ++i) {
                    ar & itu_t_t35_payload_byte[i];
                } 
            }
        };

        struct UserDataUnregistered
        {
        private:
            SeiContext const & context_;

        public:
            static int const type = 5;

            U<8> uuid_iso_iec_11578[16];
            // for( i = 16; i < payloadSize; i++ )
                std::vector<U<8> > user_data_payload_byte;

            UserDataUnregistered(
                SeiContext & context)
                : context_(context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                for (boost::uint32_t i = 0; i < 16; ++i) {
                    ar & uuid_iso_iec_11578[i];
                }
                user_data_payload_byte.resize(context_.payload_size - 16);
                for (boost::uint32_t i = 0; i < context_.payload_size - 16; ++i) {
                    ar & user_data_payload_byte[i];
                }
            }
        };

        struct RecoveryPoint
        {
            static int const type = 6;

            UE recovery_frame_cnt;
            U<1> exact_match_flag;
            U<1> broken_link_flag;
            U<2> changing_slice_group_idc;

            RecoveryPoint(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & recovery_frame_cnt
                    & exact_match_flag
                    & broken_link_flag
                    & changing_slice_group_idc;
            }
        };

        struct DecRefPicMarkingRepetition
        {
        private:
            SeiContext const & context_;
        
        public:
            static int const type = 7;

            U<1> original_idr_flag;
            UE original_frame_num;
            // if( !frame_mbs_only_flag ) {
                U<1> original_field_pic_flag;
                // if( original_field_pic_flag )
                    U<1> original_bottom_field_flag;
            // }
            // dec_ref_pic_marking()

            DecRefPicMarkingRepetition(
                SeiContext & context)
                : context_(context)
            {
            }

            SeqParameterSetRbsp const & sps() const
            {
                return context_.sps(); // TO BE FIXXED
            }
        
            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & original_idr_flag
                    & original_frame_num;
                if (!sps().frame_mbs_only_flag) {
                    ar & original_field_pic_flag;
                    if (original_field_pic_flag)
                        ar & original_bottom_field_flag;
                }
            }
        };

        struct SparePic
        {
            static int const type = 8;

            UE target_frame_num;
            U<1> spare_field_flag;
            // if( spare_field_flag )
                U<1> target_bottom_field_flag;
            UE num_spare_pics_minus1;
            // for( i = 0; i < num_spare_pics_minus1 + 1; i++ ) {
                std::vector<UE> delta_spare_frame_num;
                // if( spare_field_flag )
                    std::vector<U<1> > spare_bottom_field_flag;
                std::vector<UE> spare_area_idc;
                // if( spare_area_idc[ i ] = = 1 )
                    // for( j = 0; j < PicSizeInMapUnits; j++ )
                        std::vector<std::vector<U<1> > > spare_unit_flag;
                // else if( spare_area_idc[ i ] = = 2 ) {
                    // mapUnitCnt = 0
                    // for( j=0; mapUnitCnt < PicSizeInMapUnits; j++ ) {
                        std::vector<std::vector<UE> > zero_run_length;
                    // mapUnitCnt += zero_run_length[ i ][ j ] + 1
                    // }
                // }
            // }

            SparePic(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & target_frame_num
                    & spare_field_flag;
                if (spare_field_flag)
                    ar & target_bottom_field_flag;
                ar & num_spare_pics_minus1;
                delta_spare_frame_num.resize(num_spare_pics_minus1 + 1);
                spare_bottom_field_flag.resize(num_spare_pics_minus1 + 1);
                spare_area_idc.resize(num_spare_pics_minus1 + 1);
                spare_unit_flag.resize(num_spare_pics_minus1 + 1);
                zero_run_length.resize(num_spare_pics_minus1 + 1);
                boost::uint32_t PicSizeInMapUnits = 1; // TO BE FIXXED
                for (boost::uint32_t i = 0; i < num_spare_pics_minus1 + 1; ++i) {
                    ar & delta_spare_frame_num[i];
                    if (spare_field_flag)
                        ar & spare_bottom_field_flag[i];
                    ar & spare_area_idc[i];
                    if (spare_area_idc[i] == 1) {
                        zero_run_length[i].resize(PicSizeInMapUnits);
                        for (boost::uint32_t j = 0; j < PicSizeInMapUnits; ++j)
                            ar & spare_unit_flag[i][j];
                    } else if (spare_area_idc[i] == 2) {
                        for (boost::uint32_t l = 0; l < PicSizeInMapUnits; ) {
                            zero_run_length[i].push_back(UE());
                            ar & zero_run_length[i].back();
                            l += zero_run_length[i].back() + 1;
                        }
                    }
                }
            }
        };

        struct SceneInfo
        {
            static int const type = 9;

            U<1> scene_info_present_flag;
            // if( scene_info_present_flag ) {
                UE scene_id;
                UE scene_transition_type;
                // if( scene_transition_type > 3 )
                    UE second_scene_id;
            // }

            SceneInfo(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & scene_info_present_flag;
                if (scene_info_present_flag) {
                    ar & scene_id
                        & scene_transition_type;
                    if (scene_transition_type > 3)
                        ar & second_scene_id;
                }
            }
        };

        struct SubSeqInfo
        {
            static int const type = 10;

            UE sub_seq_layer_num;
            UE sub_seq_id;
            U<1> first_ref_pic_flag;
            U<1> leading_non_ref_pic_flag;
            U<1> last_pic_flag;
            U<1> sub_seq_frame_num_flag;
            // if( sub_seq_frame_num_flag )
                UE sub_seq_frame_num;

            SubSeqInfo(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & sub_seq_layer_num
                    & sub_seq_id
                    & first_ref_pic_flag
                    & leading_non_ref_pic_flag
                    & last_pic_flag
                    & sub_seq_frame_num_flag;
                if (sub_seq_frame_num_flag)
                    ar & sub_seq_frame_num;
            }
        };

        struct SubSeqLayerCharacteristics
        {
            static int const type = 11;

            UE num_sub_seq_layers_minus1;
            // for( layer = 0; layer <= num_sub_seq_layers_minus1; layer++ ) {
                std::vector<U<1> > accurate_statistics_flag;
                std::vector<U<16> > average_bit_rate;
                std::vector<U<16> > average_frame_rate;
            // }

            SubSeqLayerCharacteristics(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & num_sub_seq_layers_minus1;
                accurate_statistics_flag.resize(num_sub_seq_layers_minus1);
                average_bit_rate.resize(num_sub_seq_layers_minus1);
                average_frame_rate.resize(num_sub_seq_layers_minus1);
                for (boost::uint32_t i = 0; i <= num_sub_seq_layers_minus1; ++i) {
                    ar & accurate_statistics_flag[i]
                        & average_bit_rate[i]
                        & average_frame_rate[i];
                }
            }
        };

        struct SubSeqCharacteristics
        {
            static int const type = 12;

            UE sub_seq_layer_num;
            UE sub_seq_id;
            U<1> duration_flag;
            // if( duration_flag)
                U<32> sub_seq_duration;
            U<1> average_rate_flag;
            // if( average_rate_flag ) {
                U<1> accurate_statistics_flag;
                U<16> average_bit_rate;
                U<16> average_frame_rate;
            // }
            UE num_referenced_subseqs;
            // for( n = 0; n < num_referenced_subseqs; n++ ) {
                std::vector<UE> ref_sub_seq_layer_num;
                std::vector<UE> ref_sub_seq_id;
                std::vector<U<1> > ref_sub_seq_direction;
            // }

            SubSeqCharacteristics(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & sub_seq_layer_num
                    & sub_seq_id
                    & duration_flag;
                if (duration_flag)
                    ar & sub_seq_duration;
                ar & average_rate_flag;
                if (average_rate_flag) {
                    ar & accurate_statistics_flag
                        & average_bit_rate
                        & average_frame_rate;
                }
                ar & num_referenced_subseqs;
                ref_sub_seq_layer_num.resize(num_referenced_subseqs);
                ref_sub_seq_id.resize(num_referenced_subseqs);
                ref_sub_seq_direction.resize(num_referenced_subseqs);
                for (boost::uint32_t i = 0; i < num_referenced_subseqs; ++i) {
                    ar & ref_sub_seq_layer_num[i]
                        & ref_sub_seq_id[i]
                        & ref_sub_seq_direction[i];
                }
            }
        };

        struct FullFrameFreeze
        {
            static int const type = 13;

            UE full_frame_freeze_repetition_period;

            FullFrameFreeze(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & full_frame_freeze_repetition_period;
            }
        };

        struct FullFrameFreezeRelease
        {
            static int const type = 14;

            FullFrameFreezeRelease(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
            }
        };

        struct FullFrameSnapShot
        {
            static int const type = 15;

            UE snapshot_id;

            FullFrameSnapShot(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & snapshot_id;
            }
        };

        struct ProgressiveRefinementSegmentStart
        {
            static int const type = 16;

            UE progressive_refinement_id;
            UE num_refinement_steps_minus1;

            ProgressiveRefinementSegmentStart(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & progressive_refinement_id
                    & num_refinement_steps_minus1;
            }
        };

        struct ProgressiveRefinementSegmentEnd
        {
            static int const type = 17;

            UE progressive_refinement_id;

            ProgressiveRefinementSegmentEnd(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & progressive_refinement_id;
            }
        };

        struct MotionConstrainedSliceGroupSet
        {
            static int const type = 18;

            UE num_slice_groups_in_set_minus1;
            // for( i = 0; i <= num_slice_groups_in_set_minus1; i++)
                std::vector<UV<PowerMinus<DirectValue<UE> > > > slice_group_id;
            U<1> exact_sample_value_match_flag;
            U<1> pan_scan_rect_flag;
            // if( pan_scan_rect_flag )
                UE pan_scan_rect_id;

            MotionConstrainedSliceGroupSet(
                SeiContext & context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & num_slice_groups_in_set_minus1;
                UE num_slice_groups_minus1(40); // TO BE FIXXED pps.num_slice_groups_in_set_minus1
                slice_group_id.resize(num_slice_groups_in_set_minus1 + 1, power_minus(direct_value(num_slice_groups_minus1), 1));
                for (boost::uint32_t i = 0; i <= num_slice_groups_in_set_minus1; ++i)
                    ar & slice_group_id[i];
                ar & exact_sample_value_match_flag;
                ar & pan_scan_rect_flag;
                if (pan_scan_rect_flag)
                    ar & pan_scan_rect_id;
            }
        };

        struct ReservedSeiMessage
        {
        private:
            SeiContext & context_;

        public:
            static int const type = 19;

            // for( i = 0; i < payloadSize; i++ )
                std::vector<U<8> > reserved_sei_message_payload_byte;

            ReservedSeiMessage(
                SeiContext & context)
                : context_(context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                reserved_sei_message_payload_byte.resize(context_.payload_size);
                for (boost::uint32_t i = 0; i < context_.payload_size; ++i)
                    ar & reserved_sei_message_payload_byte[i];
            }
        };

        struct SeiUint
        {
            SeiUint(
                boost::uint32_t n = 0)
                : n_(n)
            {
            }

            operator boost::uint32_t() const
            {
                return n_;
            }

            boost::uint32_t n_;

            SeiUint & operator=(
                boost::uint32_t n)
            {
                n_ = n;
                return *this;
            }

            SERIALIZATION_SPLIT_MEMBER()

            template <
                typename Archive
            >
            void load(
                Archive & ar)
            {
                U<8> u8;
                do {
                    ar & u8;
                    n_ += u8;
                } while (u8 == 0xff);
            }

            template <
                typename Archive
            >
            void save(
                Archive & ar) const
            {
                boost::uint32_t u(n_);
                U<8> u8(0xff);
                while (u > 0xff) {
                    ar & u8;
                    u -= u8;
                };
                u8 = u;
                ar & u8;
            }
        };

        struct SeiPayloadLoader
        {
            SeiPayloadLoader(
                SeiContext & context)
                : context_(context)
            {
            }

            template <
                typename Payload, 
                typename Archive
            >
            void invoke(
                Archive & ar, 
                void *& payload) const
            {
                Payload * p = new Payload(context_);
                ar >> *p;
                payload = p;
            }

        private:
            SeiContext & context_;
        };

        struct SeiPayloadSaver
        {
            SeiPayloadSaver(
                SeiContext & context)
            {
            }

            template <
                typename Payload, 
                typename Archive
            >
            void invoke(
                Archive & ar, 
                void * const payload) const
            {
                Payload const * p = (Payload const *)payload;
                ar << *p;
            }
        };

        struct RawSeiMessage
        {
            SeiUint payload_type;
            SeiUint payload_size;
            std::vector<U<8> > payload_data;

            RawSeiMessage(
                SeiContext & context)
                : context_(&context)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & payload_type
                    & payload_size;

                std::cout << (Archive::is_saving::value ? "save" : "load") << " payload_type: " << (boost::uint32_t)payload_type << " payload_size: " << (boost::uint32_t)payload_size << std::endl;
                payload_data.resize(payload_size);
                for (size_t i = 0; i < payload_size; ++i) {
                    ar & payload_data[i];
                }
            }

        private:
            SeiContext * context_;
        };

        struct SeiMessage
        {
            SeiUint payload_type;
            SeiUint payload_size;

            SeiMessage(
                SeiContext & context)
                : context_(&context)
                , payload_(NULL)
            {
            }

            template <typename Payload>
            Payload & payload()
            {
                assert(payload_type == Payload::type);
                return *(Payload *)payload_;
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & payload_type
                    & payload_size;

                std::cout << "payload_type: " << (boost::uint32_t)payload_type << " payload_size: " << (boost::uint32_t)payload_size << std::endl;
                context_->payload_size = payload_size;

                typedef typename boost::mpl::if_<
                    typename Archive::is_saving, 
                    SeiPayloadSaver, 
                    SeiPayloadLoader
                >::type typex;

                typex t(*context_);
                switch (payload_type) {
                    case 0:
                        t.template invoke<BufferingPeriod>(ar, payload_);
                        break;
                    case 1:
                        t.template invoke<PicTiming>(ar, payload_);
                        break;
                    case 2:
                        t.template invoke<PanScanRect>(ar, payload_);
                        break;
                    case 3:
                        t.template invoke<FillerPayload>(ar, payload_);
                        break;
                    case 4:
                        t.template invoke<UserDataRegisteredItuTT35>(ar, payload_);
                        break;
                    case 5:
                        t.template invoke<UserDataUnregistered>(ar, payload_);
                        break;
                    case 6:
                        t.template invoke<RecoveryPoint>(ar, payload_);
                        break;
                    case 7:
                        t.template invoke<DecRefPicMarkingRepetition>(ar, payload_);
                        break;
                    case 8:
                        t.template invoke<SparePic>(ar, payload_);
                        break;
                    case 9:
                        t.template invoke<SceneInfo>(ar, payload_);
                        break;
                    case 10:
                        t.template invoke<SubSeqInfo>(ar, payload_);
                        break;
                    case 11:
                        t.template invoke<SubSeqLayerCharacteristics>(ar, payload_);
                        break;
                    case 12:
                        t.template invoke<SubSeqCharacteristics>(ar, payload_);
                        break;
                    case 13:
                        t.template invoke<FullFrameFreeze>(ar, payload_);
                        break;
                    case 14:
                        t.template invoke<FullFrameFreezeRelease>(ar, payload_);
                        break;
                    case 15:
                        t.template invoke<FullFrameSnapShot>(ar, payload_);
                        break;
                    case 16:
                        t.template invoke<ProgressiveRefinementSegmentStart>(ar, payload_);
                        break;
                    case 17:
                        t.template invoke<ProgressiveRefinementSegmentEnd>(ar, payload_);
                        break;
                    case 18:
                        t.template invoke<MotionConstrainedSliceGroupSet>(ar, payload_);
                        break;
                    default:
                        t.template invoke<ReservedSeiMessage>(ar, payload_);
                        break;
                }
                //if (!ar.byte_aligned()) {
                    U<1> bit_equal_to_one(1);
                    ar & bit_equal_to_one;
                    while (!ar.byte_aligned()) {
                        U<1> bit_equal_to_zero;
                        ar & bit_equal_to_zero;
                    }
                //}
            }

        private:
            SeiContext * context_;
            void * payload_;
        };

        struct RawSeiRbsp
            : NaluHeader
        {
            RawSeiRbsp(
                std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss, 
                std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
                : context_(spss, ppss)
            {
            }

            SERIALIZATION_SPLIT_MEMBER()

            template <
                typename Archive
            >
            void load(
                Archive & ar)
            {
                NaluHeader::serialize(ar);

                do {
                    RawSeiMessage sei_message(context_);
                    ar >> sei_message;
                    sei_messages_.push_back(sei_message);
                } while (ar.more());
            }

            template <
                typename Archive
            >
            void save(
                Archive & ar) const
            {
                //NaluHeader::serialize(ar);
                ar << (NaluHeader const &)(*this);

                for (size_t i = 0; i < sei_messages_.size(); ++i) {
                    ar << sei_messages_[i];
                }
            }

            size_t remove_type(
                int type)
            {
                size_t n = sei_messages_.size();
                sei_messages_.erase(std::remove_if(sei_messages_.begin(), sei_messages_.end(), remove_type_op(type)), 
                    sei_messages_.end());
                return n - sei_messages_.size();
            }

            void modify()
            {
                 sei_messages_[0].payload_size = 914;
            }

        private:
            struct remove_type_op
            {
                remove_type_op(int type)
                    : type(type)
                {
                }

                bool operator()(
                    RawSeiMessage const & msg)
                {
                    return msg.payload_type == type;
                }
            private:
                SeiUint type;
            };

        private:
            SeiContext context_;
            std::vector<RawSeiMessage> sei_messages_;
        };

        struct SeiRbsp
            : NaluHeader
        {
            SeiRbsp(
                std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss, 
                std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
                : context_(spss, ppss)
            {
            }

            SERIALIZATION_SPLIT_MEMBER()

            template <
                typename Archive
            >
            void load(
                Archive & ar)
            {
                NaluHeader::serialize(ar);

                do {
                    SeiMessage sei_message(context_);
                    ar >> sei_message;
                    sei_messages_.push_back(sei_message);
                } while (ar.more());
            }

            template <
                typename Archive
            >
            void save(
                Archive & ar) const
            {
                //NaluHeader::serialize(ar);
                ar << (NaluHeader const &)(*this);

                for (size_t i = 0; i < sei_messages_.size(); ++i) {
                    ar << sei_messages_[i];
                }
            }

            size_t remove_type(
                int type)
            {
                size_t n = sei_messages_.size();
                sei_messages_.erase(std::remove_if(sei_messages_.begin(), sei_messages_.end(), remove_type_op(type)), 
                    sei_messages_.end());
                return n - sei_messages_.size();
            }

            void modify()
            {
                 sei_messages_[0].payload_size = 914;
            }

        private:
            struct remove_type_op
            {
                remove_type_op(int type)
                    : type(type)
                {
                }

                bool operator()(
                    SeiMessage const & msg)
                {
                    return msg.payload_type == type;
                }
            private:
                SeiUint type;
            };

        private:
            SeiContext context_;
            std::vector<SeiMessage> sei_messages_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_SEI_TYPE_H_
