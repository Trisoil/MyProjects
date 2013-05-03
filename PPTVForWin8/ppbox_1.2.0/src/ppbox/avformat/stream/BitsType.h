// BitsType.h

#ifndef _PPBOX_AVFORMAT_STREAM_BITS_TYPE_H_
#define _PPBOX_AVFORMAT_STREAM_BITS_TYPE_H_

#include <util/serialization/Serialization.h>
#include <util/serialization/SplitMember.h>

namespace ppbox
{
    namespace avformat
    {

        inline void Exp_Golomb_Encode(
            boost::uint32_t n, 
            boost::uint32_t & l, 
            boost::uint32_t & v)
        {
            v = 1;
            while (n >= v) {
                ++l;
                v = (v << 1) | 1;
            }
            v = n - (v >> 1);
        }
        
        inline void Exp_Golomb_Decode(
            boost::uint32_t & n, 
            boost::uint32_t l, 
            boost::uint32_t v)
        {
            n = ((boost::uint32_t)1 << l) + (boost::uint32_t)v - 1;
        }
        
        template <
            size_t n
        >
        struct I
        {
            I(
                boost::int32_t i = 0)
                : i_(i)
            {
            }

            operator boost::int32_t() const
            {
                return i_;
            }

        private:
            boost::int32_t i_;
        };

        template <
            size_t n
        >
        struct U
        {
            U(
                boost::uint32_t u = 0)
                : u_(u)
            {
            }

            operator boost::uint32_t() const
            {
                return u_;
            }

        private:
            boost::uint32_t u_;
        };

        template <
            typename V
        >
        struct IV
            : V
        {
            IV(
                V const & v, 
                boost::int32_t i = 0)
                : V(v)
                , i_(i)
            {
            }

            operator boost::int32_t() const
            {
                return i_;
            }

            IV & operator=(
                boost::int32_t i)
            {
                i_ = i;
                return *this;
            }

        private:
            boost::int32_t i_;
        };

        template <
            typename V
        >
        struct UV
            : V
        {
            UV(
                V const & v, 
                boost::uint32_t u = 0)
                : V(v)
                , u_(u)
            {
            }

            operator boost::uint32_t() const
            {
                return u_;
            }

            UV & operator=(
                boost::uint32_t u)
            {
                u_ = u;
                return *this;
            }

        private:
            boost::uint32_t u_;
        };

        template <
            typename V
        >
        struct Minus
        {
            Minus(
                V const & v, 
                boost::uint32_t d = 0)
                : v_(v)
                , d_(d)
            {
            }

            boost::uint32_t operator()() const
            {
                return v_() + d_;
            }

        private:
            V v_;
            boost::uint32_t d_;
        };

        template <
            typename V
        >
        inline Minus<V> const minus(
            V const & v, 
            boost::uint32_t d)
        {
            return Minus<V>(v, d);
        }

        template <
            typename V 
        >
        struct PowerMinus
        {
            PowerMinus(
                V const & v, 
                boost::uint32_t d)
                : v_(v)
                , d_(d)
            {
            }

            boost::uint32_t operator()() const
            {
                boost::uint32_t n = v_() + d_;
                boost::uint32_t m = 1;
                boost::uint32_t s = 0;
                while (n > m) {
                    m <<= 1;
                    ++s;
                }
                return s;
            }

        private:
            V v_;
            boost::uint32_t d_;
        };

        template <
            typename V
        >
        inline PowerMinus<V> const power_minus(
            V const & v, 
            boost::uint32_t d)
        {
            return PowerMinus<V>(v, d);
        }

        struct SE
        {
            SE(
                boost::int32_t se = 0)
                : se_(se)
            {
            }

            operator boost::int32_t() const
            {
                return se_;
            }

            void parse(
                boost::uint32_t l, 
                boost::uint32_t v)
            {
                boost::uint32_t n = 0;
                Exp_Golomb_Decode(n, l, v);
                if((n & 0x01) == 0)
                    se_ = -(boost::int32_t)(n >> 1);
                else
                    se_ = boost::int32_t((n + 1) >> 1);
            }

            void format(
                boost::uint32_t & l, 
                boost::uint32_t & v) const
            {
                boost::uint32_t n = (se_ > 0) ? ((se_ << 1) - 1) : ((-se_) << 1);
                Exp_Golomb_Encode(n, l, v);
            }

            boost::uint32_t format_size() const
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                format(l, v);
                return l * 2 + 1;
            }

            boost::uint32_t format_value() const
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                format(l, v);
                return v | (1 << l);
            }

        private:
            boost::int32_t se_;
        };

        struct UE
        {
            UE(
                boost::uint32_t ue = 0)
                : ue_(ue)
            {
            }

            operator boost::uint32_t() const
            {
                return ue_;
            }

            void parse(
                boost::uint32_t l, 
                boost::uint32_t v)
            {
                Exp_Golomb_Decode(ue_, l, v);
            }

            void format(
                boost::uint32_t & l, 
                boost::uint32_t & v) const
            {
                Exp_Golomb_Encode(ue_, l, v);
            }

            boost::uint32_t format_size() const
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                format(l, v);
                return l * 2 + 1;
            }

            boost::uint32_t format_value() const
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                format(l, v);
                return v | (1 << l);
            }

        private:
            boost::uint32_t ue_;
        };

        struct P // rbsp trailing bits
        {
            void data(
                boost::uint32_t n) const
            {
                n_ = n;
            }

            boost::uint32_t data() const
            {
                return n_;
            }

            void size(
                boost::uint32_t s) const
            {
                s_ = s;
            }

            boost::uint32_t size() const
            {
                return s_;
            }

        private:
            mutable boost::uint32_t n_;
            mutable boost::uint32_t s_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_STREAM_BITS_TYPE_H_
