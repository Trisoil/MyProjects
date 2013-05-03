// AvcSliceType.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_SLICE_TYPE_H_
#define _PPBOX_AVFORMAT_AVC_AVC_SLICE_TYPE_H_

#include "ppbox/avformat/codec/avc/AvcNalu.h"
#include "ppbox/avformat/codec/avc/AvcSpsPpsType.h"

namespace ppbox
{
    namespace avformat
    {

       struct SliceHeader
       {
       private:
           NaluHeader const & header_;
           std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss_;

       public:
           UE first_mb_in_slice;
           UE slice_type;
           UE pic_parameter_set_id;
           UV<Minus<ConstMemFunc<SliceHeader, UE const &> > > frame_num; // log2_max_frame_num_minus4
           // if (!frame_mbs_only_flag) {
               U<1> field_pic_flag;
               // if (field_pic_flag)
                   U<1> bottom_field_flag;
           // }
           // if (nal_unit_type == 5)
               UE idr_pic_id;
           // if (pic_order_cnt_type == 0) {
               UV<Minus<ConstMemFunc<SliceHeader, UE const &> > > pic_order_cnt_lsb; // log2_max_pic_order_cnt_lsb_minus4
               // if (pic_order_present_flag && !field_pic_flag)
                   SE delta_pic_order_cnt_bottom;
           // }

           SliceHeader(
               NaluHeader const & header, 
               std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
               : header_(header)
               , ppss_(ppss)
               , frame_num(minus(mem_func(this, &SliceHeader::log2_max_frame_num_minus4), 4))
               , pic_order_cnt_lsb(minus(mem_func(this, &SliceHeader::log2_max_pic_order_cnt_lsb_minus4), 4))
           {
           }

           UE const & log2_max_frame_num_minus4() const
           {
               return sps().log2_max_frame_num_minus4;
           }

           UE const & log2_max_pic_order_cnt_lsb_minus4() const
           {
               return sps().log2_max_pic_order_cnt_lsb_minus4;
           }

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               ar & first_mb_in_slice
                   & slice_type
                   & pic_parameter_set_id
                   & frame_num;
               assert(ppss_.find(pic_parameter_set_id) != ppss_.end());
               if (ppss_.find(pic_parameter_set_id) == ppss_.end()) {
                   ar.failed();
                   return;
               }
               if (sps().frame_mbs_only_flag) {
                   ar & field_pic_flag;
                   if (field_pic_flag)
                       ar & bottom_field_flag;
               }
               if (header_.nal_unit_type == 5) {
                   ar & idr_pic_id;
               }
               if (sps().pic_order_cnt_type == 0) {
                   ar & pic_order_cnt_lsb;
                   if (pps().pic_order_present_flag && !field_pic_flag)
                       ar & delta_pic_order_cnt_bottom;
               }
           }

           std::map<boost::uint32_t, SeqParameterSetRbsp> const & spss() const
           {
               return (*ppss_.begin()).second.spss();;
           }

           std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss() const
           {
               return ppss_;
           }

           SeqParameterSetRbsp const & sps() const
           {
               return pps().sps();
           }

           PicParameterSetRbsp const & pps() const
           {
               return (*ppss_.find(pic_parameter_set_id)).second;
           }

           static char const * const slice_type_str[10];
       };

       struct SliceData {};

       struct RbspSliceTrailingBits {};

       // nal_unit_type == 1
       // Coded slice of a non-IDR picture
       // nal_unit_type == 5
       // Coded slice of a IDR picture
       struct SliceLayerWithoutPartitioningRbsp
           : NaluHeader
       {
           SliceHeader slice_header;
           SliceData slice_data; /* all categories of slice_data( ) syntax */
           RbspSliceTrailingBits rbsp_slice_trailing_bits;

           SliceLayerWithoutPartitioningRbsp(
               std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
               : slice_header(*this, ppss)
           {
           }

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               NaluHeader::serialize(ar);

               ar & slice_header;
           }
       };

       // nal_unit_type == 2
       // Coded slice data partition A
       struct SliceDataPartitioningALayerRbsp
           : NaluHeader
       {
           SliceHeader slice_header;
           UE slice_id;
           SliceData slice_data; /* only category 2 parts of slice_data( ) syntax */
           RbspSliceTrailingBits rbsp_slice_trailing_bits;

           SliceDataPartitioningALayerRbsp(
               std::map<boost::uint32_t, PicParameterSetRbsp> const & ppss)
               : slice_header(*this, ppss)
           {
           }

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               NaluHeader::serialize(ar);

               ar & slice_header;
                   & slice_id;
           }
       };

       // nal_unit_type == 3
       // Coded slice data partition B
       struct SliceDataPartitioningBLayerRbsp
           : NaluHeader
       {
           UE slice_id;
           // if (redundant_pic_cnt_present_flag)
               UE redundant_pic_cnt;
           SliceData slice_data; /* only category 3 parts of slice_data( ) syntax */
           RbspSliceTrailingBits rbsp_slice_trailing_bits;

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               NaluHeader::serialize(ar);

               ar & slice_id;
           }
       };

       // nal_unit_type == 4
       // Coded slice data partition C
       struct SliceDataPartitioningCLayerRbsp
           : NaluHeader
       {
           UE slice_id;
           // if (redundant_pic_cnt_present_flag)
               UE redundant_pic_cnt;
           SliceData slice_data; /* only category 4 parts of slice_data( ) syntax */
           RbspSliceTrailingBits rbsp_slice_trailing_bits;

           template <
               typename Archive
           >
           void serialize(
               Archive & ar)
           {
               NaluHeader::serialize(ar);

               ar & slice_id;
           }
       };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_SLICE_TYPE_H_
