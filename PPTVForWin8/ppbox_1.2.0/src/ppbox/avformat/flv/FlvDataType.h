// FlvDataType.h

#ifndef _PPBOX_AVFORMAT_FLV_FLV_DATA_TYPE_H_
#define _PPBOX_AVFORMAT_FLV_FLV_DATA_TYPE_H_

#include "ppbox/avformat/flv/FlvFormat.h"

namespace ppbox
{
    namespace avformat
    {

        struct FlvDataType // AMF Data Type
        {
            enum Enum {
                NUMBER      = 0x00,
                BOOL        = 0x01,
                STRING      = 0x02,
                OBJECT      = 0x03,
                MOVIECLIP   = 0x04,
                _NULL       = 0x05,
                UNDEFINED   = 0x06,
                REFERENCE   = 0x07,
                MIXEDARRAY  = 0x08,
                OBJECT_END  = 0x09,
                ARRAY       = 0x0a,
                DATE        = 0x0b,
                LONG_STRING = 0x0c,
                UNSUPPORTED = 0x0d,
            };
        };

        struct FlvDataString
        {
            boost::uint16_t StringLength;
            std::string StringData;

            FlvDataString()
                : StringLength(0)
            {
            }

            FlvDataString(
                char const * str)
                : StringLength(strlen(str))
                , StringData(str)
            {
            }

            FlvDataString(
                std::string const & str)
                : StringLength(str.size())
                , StringData(str)
            {
            }

            friend bool operator==(
                FlvDataString const & l, 
                FlvDataString const & r)
            {
                return l.StringData == r.StringData;
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & StringLength;
                util::serialization::serialize_collection(ar, StringData, StringLength);
            }
        };

        struct FlvDataDate
        {
            double DateTime;
            boost::int16_t LocalDateTimeOffset;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & DateTime;
                ar & LocalDateTimeOffset;
            }
        };

        struct FlvDataLongString
        {
            boost::uint32_t StringLength;
            std::vector<boost::uint8_t> StringData;

            FlvDataLongString()
                : StringLength(0)
            {
            }

            FlvDataLongString(
                std::vector<boost::uint8_t> const & data)
                : StringLength(data.size())
                , StringData(data)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & StringLength;
                util::serialization::serialize_collection(ar, StringData, StringLength);
            }
        };

        struct FlvDataObjectProperty;

        static inline bool FLV_Property_End(
            FlvDataObjectProperty const & Property);

        struct FlvDataObject
        {
            std::vector<FlvDataObjectProperty> ObjectProperties;

            SERIALIZATION_SPLIT_MEMBER();

            template <typename Archive>
            void load(
                Archive & ar)
            {
                FlvDataObjectProperty Property;
                ObjectProperties.clear();
                while (ar & Property) {
                    ObjectProperties.push_back(Property);
                    if (FLV_Property_End(Property))
                        break;
                }
            }

            template <typename Archive>
            void save(
                Archive & ar) const
            {
                for (size_t i = 0; i < ObjectProperties.size(); ++i) {
                    ar & ObjectProperties[i];
                }
                ar & FlvDataObjectProperty();
            }
        };

        struct FlvDataECMAArray
        {
            boost::uint32_t ECMAArrayLength;
            std::vector<FlvDataObjectProperty> Variables;

            SERIALIZATION_SPLIT_MEMBER();

            template <typename Archive>
            void load(
                Archive & ar)
            {
                ar & ECMAArrayLength;
                FlvDataObjectProperty Property;
                Variables.clear();
                while (ar & Property) {
                    Variables.push_back(Property);
                    if (FLV_Property_End(Property))
                        break;
                }
            }

            template <typename Archive>
            void save(
                Archive & ar) const
            {
                boost::uint32_t ECMAArrayLength = Variables.size();
                ar & ECMAArrayLength;
                for (size_t i = 0; i < Variables.size(); ++i) {
                    ar & Variables[i];
                }
                ar & FlvDataObjectProperty();
            }
        };

        struct FlvDataValue;

        struct FlvDataStrictArray
        {
            boost::uint32_t StrictArrayLength;
            std::vector<FlvDataValue> StrictArrayValue;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & StrictArrayLength;
                util::serialization::serialize_collection(ar, StrictArrayValue, StrictArrayLength);
            }
        };

        struct FlvDataValue
        {
            boost::uint8_t Type;
            union {
                double Double;
                boost::uint64_t _Double;
                boost::uint8_t Bool;
                boost::uint16_t MovieClip;
                boost::uint16_t Null;
                boost::uint16_t Undefined;
                boost::uint16_t Reference;
                boost::uint16_t ObjectEndMarker;
                FlvDataDate Date;
            };
            FlvDataString String;
            FlvDataLongString LongString;
            FlvDataObject Object;
            FlvDataECMAArray ECMAArray;
            FlvDataStrictArray StrictArray;

            FlvDataValue(
                FlvDataType::Enum type = FlvDataType::UNDEFINED)
                : Type(type)
            {
            }

            FlvDataValue(
                double num)
                : Type(FlvDataType::NUMBER)
                , Double(num)
            {
            }

            //FlvDataValue(
            //    bool b)
            //    : Type(FlvDataType::BOOL)
            //    , Bool(b)
            //{
            //}

            FlvDataValue(
                char const * str)
                : Type(FlvDataType::STRING)
                , String(str)
            {
            }

            FlvDataValue(
                std::string const & str)
                : Type(FlvDataType::STRING)
                , String(str)
            {
            }

            FlvDataValue(
                std::vector<boost::uint8_t> const & data)
                : Type(FlvDataType::LONG_STRING)
                , LongString(data)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & Type;
                switch (Type) {
                    case FlvDataType::NUMBER:
                        ar & _Double;
                        break;
                    case FlvDataType::BOOL:
                        ar & Bool;
                        break;
                    case FlvDataType::STRING:
                        ar & String;
                        break;
                    case FlvDataType::OBJECT:
                        ar & Object;
                        break;
                    case FlvDataType::MOVIECLIP:
                        ar.fail();
                        break;
                    case FlvDataType::_NULL:
                        break;
                    case FlvDataType::UNDEFINED:
                        break;
                    case FlvDataType::REFERENCE:
                        ar & Reference;
                        break;
                    case FlvDataType::MIXEDARRAY:
                        ar & ECMAArray;
                        break;
                    case FlvDataType::OBJECT_END:
                        break;
                    case FlvDataType::ARRAY:
                        ar & StrictArray;
                        break;
                    case FlvDataType::DATE:
                        ar & Date;
                        break;
                    case FlvDataType::LONG_STRING:
                        ar & LongString;
                        break;
                    default:
                        ar.fail();
                        break;
                }
            }
        };

        struct FlvDataObjectProperty
        {
            FlvDataString PropertyName;
            FlvDataValue PropertyData;

            FlvDataObjectProperty() 
                : PropertyData(FlvDataType::OBJECT_END)
            {
            }

            template <typename T>
            FlvDataObjectProperty(
                std::string const & name, 
                T const & value)
                : PropertyName(name)
                , PropertyData(value)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & PropertyName
                    & PropertyData;
            }
        };

        static inline bool FLV_Property_End(
            FlvDataObjectProperty const & Property)
        {
            return Property.PropertyData.Type == FlvDataType::OBJECT_END;
        }

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_FLV_FLV_DATA_TYPE_H_
