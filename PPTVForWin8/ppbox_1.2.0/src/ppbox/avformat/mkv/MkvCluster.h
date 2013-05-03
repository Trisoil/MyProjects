// MkvCluster.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_CLUSTER_H_
#define _PPBOX_AVFORMAT_MKV_MKV_CLUSTER_H_

namespace ppbox
{
    namespace avformat
    {

        // Block: child of BlockGroup

        struct MkvBlockData
        {
            vint TrackNumber;
            boost::int16_t TimeCode;

            enum LaceTypeEnum
            {
                no_lacing, 
                xiph_lacing, 
                fixed_size_lacing, 
                ebml_lacing, 
            };

            union {
                struct {
#ifdef   BOOST_LITTLE_ENDIAN
                    boost::uint8_t Discardable : 1;
                    boost::uint8_t LaceType : 2;
                    boost::uint8_t Invisible : 1;
                    boost::uint8_t Reserved : 3;
                    boost::uint8_t Keyframe : 1;
#else 
                    boost::uint8_t Keyframe : 1;
                    boost::uint8_t Reserved : 3;
                    boost::uint8_t Invisible : 1;
                    boost::uint8_t LaceType : 2;
                    boost::uint8_t Discardable : 1;
#endif
                };
                boost::uint8_t Flag;
            };

            // if (lacing)
                boost::uint8_t frame_count_minus_one;
                // if (lacaing == ebml_lacing)
                    // vint ebml_size0;
                    // std::vector<svint> ebml_sizes;
                // else if lcaing == Xiph lacing
                    // std::vector<boost::uint8_t> xiph_sizes;
                std::vector<boost::uint32_t> sizes;


            MkvBlockData()
                : TimeCode(0)
                , Flag(0)
            {
            }

            SERIALIZATION_SPLIT_MEMBER();

            template <typename Archive>
            void save(
                Archive & ar) const
            {
                ar & TrackNumber
                    & TimeCode
                    & Flag;

                if (LaceType) {
                    ar & frame_count_minus_one;
                    if (LaceType == ebml_lacing) {
                        vint size = sizes[0];
                        ar & size;
                        for (size_t i = 1; i < (size_t)frame_count_minus_one; ++i) {
                            svint size = (boost::int32_t)(sizes[i] - sizes[i - 1]);
                            ar & size;
                        }
                    } else if (LaceType == xiph_lacing) {
                        for (size_t i = 0; i < (size_t)frame_count_minus_one + 1; ++i) {
                            boost::uint32_t size = sizes[i];
                            boost::uint8_t size255 = 255;
                            while (size >= 255) {
                                ar & size255;
                                size -= 255;
                            }
                            ar & size;
                        }
                    }
                }
            }

            template <typename Archive>
            void load(
                Archive & ar)
            {
                ar & TrackNumber
                    & TimeCode
                    & Flag;

                if (LaceType) {
                    ar & frame_count_minus_one;
                    sizes.resize(frame_count_minus_one + 1);
                    if (LaceType == ebml_lacing) {
                        boost::uint32_t total = 0;
                        vint size;
                        ar & size;
                        sizes[0] = size;
                        total += size;
                        for (size_t i = 1; i < (size_t)frame_count_minus_one; ++i) {
                            svint size;
                            ar & size;
                            sizes[i] = sizes[i - 1] + size;
                            total += sizes[i];
                        }
                        sizes.back() = total;
                    } else if (LaceType == xiph_lacing) {
                        for (size_t i = 0; i < (size_t)frame_count_minus_one + 1; ++i) {
                            boost::uint8_t size;
                            ar & size;
                            while (size >= 255) {
                                sizes[i] += 255;
                                ar & size;
                            }
                            sizes[i] += size;
                        }
                    }
                }
            }

            void compelte_load(
                boost::uint32_t total)
            {
                if (LaceType == no_lacing) {
                    sizes.push_back(total);
                } else if (LaceType == ebml_lacing) {
                    sizes.back() = total - sizes.back();
                } else if (LaceType == xiph_lacing) {
                } else {
                    boost::uint32_t size = total / (frame_count_minus_one + 1);
                    sizes.assign(sizes.size(), size);
                }
            }

            /*
            boost::uint32_t size() const
            {
                return TrackNumber.size() + 3;
            }*/
        };

        typedef EBML_Element<0x21, MkvBlockData> MkvBlock;

        // BlockGroup: child of Cluster

        struct MkvBlockGroupData
        {
            EBML_Int ReferenceBlock;
            EBML_Int BlockDuration;
            MkvBlock Block;

            MkvBlockGroupData()
                : ReferenceBlock(0x7B)
                , BlockDuration(0x1B)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & ReferenceBlock
                    & BlockDuration
                    & Block;
            }
        };

        typedef EBML_Element<0x20, MkvBlockGroupData> MkvBlockGroup;

        // SimpleBlock: child of Cluster

        typedef EBML_Element<0x23, MkvBlockData> MkvSimpleBlock;

        // Cluster: child of Segment

        struct MkvClusterData
        {
            EBML_UInt TimeCode;
            EBML_UInt Position;
            EBML_UInt PrevSize;

            std::vector<MkvSimpleBlock> SimpleBlocks;
            std::vector<MkvBlockGroup> BlockGroups;

            MkvClusterData()
                : TimeCode(0x67)
                , Position(0x27)
                , PrevSize(0x2B)
            {
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & TimeCode
                    & Position
                    & PrevSize;
                ar & SimpleBlocks;
                ar & BlockGroups;
            }
        };

        typedef EBML_Element<0xF43B675, MkvClusterData> MkvCluster;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_MKV_CLUSTER_H_
