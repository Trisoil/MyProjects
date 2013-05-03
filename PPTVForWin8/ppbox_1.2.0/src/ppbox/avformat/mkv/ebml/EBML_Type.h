// EBML_Type.h

#ifndef _PPBOX_AVFORMAT_MKV_EBML_EBML_TYPE_H_
#define _PPBOX_AVFORMAT_MKV_EBML_EBML_TYPE_H_

#include <util/serialization/stl/vector.h>
#include <util/serialization/SplitMember.h>
#include <util/serialization/VariableNumber.h>
#include <util/serialization/Array.h>
#include <util/archive/BigEndianBinaryIArchive.h>
#include <util/archive/BigEndianBinaryOArchive.h>

#include <framework/system/VariableNumber.h>

namespace ppbox
{
    namespace avformat
    {

        typedef util::archive::BigEndianBinaryOArchive<boost::uint8_t> EBML_OArchive;

        typedef util::archive::BigEndianBinaryIArchive<boost::uint8_t> EBML_IArchive;

        typedef framework::system::VariableNumber<boost::uint32_t> vint;

        typedef framework::system::VariableNumber<boost::uint64_t> vint64;

        typedef framework::system::VariableNumber<boost::int32_t> svint;

        //EBML
        struct EBML_ElementHeader
        {
            vint Id;
            vint64 Size;

            EBML_ElementHeader()
            {
            }

            EBML_ElementHeader(
                boost::uint32_t id)
                : Id(id)
            {
            }

            EBML_ElementHeader(
                boost::uint32_t id,
                boost::uint64_t size)
                : Id(id)
                , Size(size)
            {
            }

            template <
                typename Archive
            >
            void serialize(
                Archive & ar)
            {
                ar & Id
                    & Size;
            }

            void clear()
            {
                Size = vint64();
            }

            bool empty() const
            {
                return Size == vint64();
            }

            boost::uint64_t head_size() const
            {
                return empty() ? 0 : Id.size() + Size.size();
            }

            boost::uint64_t data_size() const
            {
                return Size.is_unknown() ? boost::uint64_t(0x8000000000000000ULL) : boost::uint64_t(Size);
            }

            boost::uint64_t byte_size() const
            {
                return empty() ? 0 : Id.size() + Size.size() + Size;
            }
        };

        template <
            typename _Ty
        >
        struct EBML_UnitTraits
        {
            static boost::uint32_t calc_size(
                boost::int64_t n)
            {
                return sizeof(_Ty);
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                _Ty const & v, 
                size_t n)
            {
                ar << v;
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                _Ty & v, 
                size_t n)
            {
                ar >> v;
            }
        };

        template <
            typename _Ty, 
            typename _Traits = EBML_UnitTraits<_Ty>
        >
        struct EBML_UnitType
            : EBML_ElementHeader
        {
        public:
            explicit EBML_UnitType(
                boost::uint32_t id)
                : EBML_ElementHeader(id)
                , v_(_Ty())
            {
            }

            EBML_UnitType(
                boost::uint32_t id,
                _Ty const & v)
                : EBML_ElementHeader(id, _Traits::calc_size(v))
                , v_(v)
            {
            }

        public:
            EBML_UnitType & operator=(
                _Ty const & v)
            {
                v_ = v;
                this->Size = _Traits::calc_size(v_);
                return *this;
            }

            bool operator ==(
                _Ty const & v) const
            {
                return v_ == v;
            }

            _Ty & value()
            {
                return v_;
            }

            _Ty const & value() const
            {
                return v_;
            }

            _Ty const & value_or(
                _Ty const & o) const
            {
                return empty() ? o : v_;
            }

        public:
            SERIALIZATION_SPLIT_MEMBER();

            template <typename Archive>
            void save(
                Archive & ar) const
            {
                if (empty())
                    return;
                ar << (EBML_ElementHeader const &)(*this);
                _Traits::save(ar, const_cast<_Ty const &>(v_), (size_t)Size.value());
            }

            template <typename Archive>
            void load(
                Archive & ar)
            {
                EBML_ElementHeader::serialize(ar);
                if (empty())
                    return;
                _Traits::load(ar, v_, (size_t)Size.value());
            }

            template <typename Archive>
            void load_value(
                Archive & ar)
            {
                _Traits::load(ar, v_, (size_t)Size.value());
            }

        private:
            _Ty v_;
        };

        template <>
        struct EBML_UnitTraits<boost::uint64_t>
        {
             // 计算当前数据内容大小
            static boost::uint32_t calc_size(
                boost::uint64_t n)
            {
                //须由高位至地位比较
                if (0xFFFFFFFF00000000ULL & n) {
                    if(0xFFFF000000000000ULL & n) {
                        if(0xFF00000000000000ULL & n)
                            return 8;
                        else
                            return 7;
                    } else {
                        if(0xFF0000000000ULL & n)
                            return 6;
                        else
                            return 5;
                    }
                } else {
                    if(0xFFFF0000ULL & n) {
                        if(0xFF000000ULL & n)
                            return 4;
                        else
                            return 3;
                    } else {
                        if(0xFF00ULL & n)
                            return 2;
                        else
                            return 1;
                    }
                }
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                boost::uint64_t const & v, 
                size_t n)
            {
                boost::uint64_t t = 
                    framework::system::BytesOrder::host_to_big_endian(v);
                ar << framework::container::make_array(
                    (boost::uint8_t *)&t + 8 - n, n);
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                boost::uint64_t & v, 
                size_t n)
            {
                boost::uint64_t t = 0;
                ar >> framework::container::make_array(
                    (boost::uint8_t *)&t + 8 - n, n);
                v = framework::system::BytesOrder::host_to_big_endian(t);
            }
        };

        template <>
        struct EBML_UnitTraits<boost::int64_t>
        {
            static boost::uint32_t calc_size(
                boost::int64_t n)
            {
                if(0x8000000000000000ULL & n) {
                    return EBML_UnitTraits<boost::uint64_t>::calc_size(n & 0x7FFFFFFFFFFFFFFFULL) + 1;
                } else {
                    return EBML_UnitTraits<boost::uint64_t>::calc_size(n) + 1;
                }
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                boost::int64_t const & v, 
                size_t n)
            {
                boost::int64_t t = 
                    framework::system::BytesOrder::host_to_big_endian(v);
                ar << framework::container::make_array(
                    (boost::uint8_t *)&t + 8 - n, n);
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                boost::int64_t & v, 
                size_t n)
            {
                boost::int64_t t = 0;
                ar >> framework::container::make_array(
                    (boost::uint8_t *)&t + 8 - n, n);
                v = framework::system::BytesOrder::host_to_big_endian(t);
            }
        };

        template <>
        struct EBML_UnitTraits<std::string>
        {
            static boost::uint32_t calc_size(
                std::string const & s)
            {
                return s.size();
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                std::string const & s, 
                size_t n)
            {
                ar & framework::container::make_array(s.c_str(), n);
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                std::string & s, 
                size_t n)
            {
                s.resize(n);
                ar & framework::container::make_array(s.c_str(), n);
            }
        };

        struct EBML_FloatOrDouble
        {
        public:
            EBML_FloatOrDouble(
                float f = 0.0f)
                : t_(false)
                , f_(f)
            {
            }

            EBML_FloatOrDouble(
                double d)
                : t_(true)
                , d_(d)
            {
            }

            bool is_double() const
            {
                return t_;
            }

            float & float_value()
            {
                return f_;
            }

            double & double_value()
            {
                return d_;
            }

            float const & float_value() const
            {
                return f_;
            }

            double const & double_value() const
            {
                return d_;
            }

            boost::int64_t as_int64() const
            {
                return t_ ? (boost::int64_t)d_ : (boost::int64_t)f_;
            }

            boost::int32_t as_int32() const
            {
                return t_ ? (boost::int32_t)d_ : (boost::int32_t)f_;
            }

        private:
            bool t_;
            union
            {
                float f_;
                double d_;
            };
        };

        template <>
        struct EBML_UnitTraits<EBML_FloatOrDouble>
        {
            static boost::uint32_t calc_size(
                EBML_FloatOrDouble const & v)
            {
                return v.is_double() ? 8 : 4;
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                EBML_FloatOrDouble const & v, 
                size_t n)
            {
                if (v.is_double())
                    ar << v.double_value();
                else
                    ar << v.float_value();
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                EBML_FloatOrDouble & v, 
                size_t n)
            {
                if (v.is_double()) {
                    ar >> v.double_value();
                } else {
                    ar >> v.float_value();
                }
            }
        };

        template <>
        struct EBML_UnitTraits<std::vector<boost::uint8_t> >
        {
            static boost::uint32_t calc_size(
                std::vector<boost::uint8_t> const & v)
            {
                return v.size();
            }

            template <typename Archive>
            static void save(
                Archive & ar, 
                std::vector<boost::uint8_t> const & v, 
                size_t n)
            {
                if (n > 0)
                    ar & framework::container::make_array(&v[0], n);
            }

            template <typename Archive>
            static void load(
                Archive & ar, 
                std::vector<boost::uint8_t> & v, 
                size_t n)
            {
                v.resize(n);
                if (n > 0)
                    ar & framework::container::make_array(&v[0], n);
            }
        };

        typedef EBML_UnitType<boost::uint64_t> EBML_UInt;
        typedef EBML_UnitType<boost::int64_t> EBML_Int;
        typedef EBML_UnitType<std::string> EBML_String;
        typedef EBML_UnitType<boost::uint8_t> EBML_Bool;
        typedef EBML_UnitType<EBML_FloatOrDouble> EBML_Float;
        typedef EBML_UnitType<std::vector<boost::uint8_t> > EBML_Binary;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_EBML_EBML_TYPE_H_
