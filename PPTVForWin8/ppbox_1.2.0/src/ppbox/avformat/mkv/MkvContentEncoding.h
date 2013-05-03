// MkvContentEncoding.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_CONTENT_ENCODING_H_
#define _PPBOX_AVFORMAT_MKV_MKV_CONTENT_ENCODING_H_

namespace ppbox
{
    namespace avformat
    {

        // ContentCompression: child of ContentEncoding

        struct MkvContentCompressionData
        {
            EBML_UInt ContentCompAlgo;
            EBML_Binary ContentCompSettings;

            MkvContentCompressionData()
                : ContentCompAlgo(0x254)
                , ContentCompSettings(0x255)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & ContentCompAlgo
                    & ContentCompSettings;
            }
        };

        typedef EBML_Element<0x1034, MkvContentCompressionData> MkvContentCompression;

        // ContentEncryption: child of ContentEncoding

        // typedef EBML_Element<0x1035, MkvContentEncryptionData> MkvContentEncryption;

        // ContentEncoding: child of ContentEncodings

        struct MkvContentEncodingData
        {
            EBML_UInt ContentEncodingOrder;
            EBML_UInt ContentEncodingScope;
            EBML_UInt ContentEncodingType;

            MkvContentCompression ContentCompression;
            //MkvContentEncryption ContentEncryption;

            MkvContentEncodingData()
                : ContentEncodingOrder(0x1031)
                , ContentEncodingScope(0x1032)
                , ContentEncodingType(0x1033)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & ContentEncodingOrder
                    & ContentEncodingScope
                    & ContentEncodingType
                    & ContentCompression;
                    //& ContentEncryption;
            }
        };

        typedef EBML_Element<0x2240, MkvContentEncodingData> MkvContentEncoding;

        // ContentEncodings: child of TrackEntry

        struct MkvContentEncodingsData
        {
            std::vector<MkvContentEncoding> ContentEncodings;

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & ContentEncodings;
            }
        };

        typedef EBML_Element<0x2D80, MkvContentEncodingsData> MkvContentEncodings;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_MKV_CONTENT_ENCODING_H_
