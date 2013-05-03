// AvcNalu.h

#ifndef _PPBOX_AVFORMAT_AVC_AVC_NALU_H_
#define _PPBOX_AVFORMAT_AVC_AVC_NALU_H_

#include "ppbox/avformat/stream/BitsType.h"

namespace ppbox
{
    namespace avformat
    {

        struct NaluHeader
        {
            enum NaluTypeEnum
            {
                undefine = 0,
                UNIDR,
                DATABLOCK_A,
                DATABLOCK_B,
                DATABLOCK_C,
                IDR,
                SEI,
                SPS,
                PPS,
                AccessUnitDelimiter,
                EndOfSeq,
                EndOfStream,
                FillerData,
                SPSExtension,
                Other,
            };

            U<1> forbidden_zero_bit;
            U<2> nal_ref_idc;
            U<5> nal_unit_type;

            NaluHeader(
                U<5> const & nal_unit_type = 0)
                : nal_unit_type(nal_unit_type)
            {
            }

            NaluHeader(
                boost::uint8_t b)
            {
                nal_ref_idc = b & 0x60;
                nal_unit_type = b & 0x1f;
            }

            boost::uint8_t byte() const
            {
                return (boost::uint8_t)nal_ref_idc << 5 | (boost::uint8_t)nal_unit_type;
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & forbidden_zero_bit
                    & nal_ref_idc
                    & nal_unit_type;
            }

            static char const * const nalu_type_str[];
        };

       template <
           typename V
       >
       struct DirectValue
       {
           DirectValue(
               V const & v)
               : v_(&v)
           {
           }

           V const & operator()() const
           {
               return *v_;
           }

       private:
           V const * v_;
       };

       template <
           typename V
       >
       inline DirectValue<V> const direct_value(
           V const & v)
       {
           return DirectValue<V>(v);
       }

       template <
           typename Object, 
           typename Result
       >
       struct ConstMemFunc
       {
           ConstMemFunc(
               Object const * obj, 
               Result (Object::*func)() const)
               : obj_(obj)
               , func_(func)
           {
           }

           Result operator()() const
           {
               return (obj_->*func_)();
           }

       private:
           Object const * obj_;
           Result (Object::* func_)() const;
       };

       template <
           typename Object, 
           typename Result
       >
       inline ConstMemFunc<Object, Result> const mem_func(
           Object const * obj, 
           Result (Object::*func)() const)
       {
           return ConstMemFunc<Object, Result>(obj, func);
       }

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVC_AVC_NALU_H_
