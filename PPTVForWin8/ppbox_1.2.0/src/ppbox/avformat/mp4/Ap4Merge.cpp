// Mp4Merge.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/mp4/Ap4Merge.h"
#include "ppbox/avformat/mp4/Ap4Error.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <bento4/Core/Ap4.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.avformat.Ap4Merge", framework::logger::Debug);

namespace ppbox
{
    namespace avformat
    {

        /* Movie header atom (/moov/mvhd)
        *  .....
        *  Duration
        *  .....
        */
        static bool ap4_merge_mvhd(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_MvhdAtom * mvhd1 = (AP4_MvhdAtom *)atom1;
            AP4_MvhdAtom * mvhd2 = (AP4_MvhdAtom *)atom2;
            mvhd1->SetDuration(mvhd1->GetDuration() + mvhd2->GetDuration());
            mvhd1->GetParent()->OnChildChanged(mvhd1);
            return true;
        }

        /* Track header atom (/moov/trak/tkhd)
        *  .....
        *  Duration
        *  .....
        */
        static bool ap4_merge_track_tkhd(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_TkhdAtom * tkhd1 = (AP4_TkhdAtom *)atom1;
            AP4_TkhdAtom * tkhd2 = (AP4_TkhdAtom *)atom2;
            tkhd1->SetDuration(tkhd1->GetDuration() + tkhd2->GetDuration());
            tkhd1->GetParent()->OnChildChanged(tkhd1);
            return true;
        }

        /* Media header atom (/moov/trak/mdia/mdhd)
        *  .....
        *  Duration
        *  .....
        */
        static bool ap4_merge_track_mdhd(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_MdhdAtom * mdhd1 = (AP4_MdhdAtom *)atom1;
            AP4_MdhdAtom * mdhd2 = (AP4_MdhdAtom *)atom2;
            mdhd1->SetDuration(mdhd1->GetDuration() + mdhd2->GetDuration());
            mdhd1->GetParent()->OnChildChanged(mdhd1);
            return true;
        }

        /* Time-to-sample atom (/moov/trak/minf/stbl/stts)
        *  .....
        *  Number of entries
        *  Time-to-sample table
        *      Time-to-sample table entry
        *          Sample count
        *          Sample duration
        *  .....
        */
        static bool ap4_merge_track_stts(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_SttsAtom * stts1 = (AP4_SttsAtom *)atom1;
            AP4_SttsAtom * stts2 = (AP4_SttsAtom *)atom2;
            AP4_Array<AP4_SttsTableEntry> const & stts_tbl = stts2->GetEntries();
            for (AP4_Ordinal i = 0; i < stts_tbl.ItemCount(); ++i) {
                stts1->AddEntry(stts_tbl[i].m_SampleCount, stts_tbl[i].m_SampleDuration);
            }
            stts1->GetParent()->OnChildChanged(stts1);
            return true;
        }

        /* Composition-time-to-sample atom (/moov/trak/minf/stbl/ctts)
        *  .....
        *  Number of entries
        *  Composition-Time-to-sample table
        *      Composition-Time-to-sample table entry
        *          Sample count
        *          Sample duration
        *      Composition-Time-to-sample table entry
        *          Sample count
        *          Sample duration
        *  .....
        */
        static bool ap4_merge_track_ctts(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_CttsAtom * ctts1 = (AP4_CttsAtom *)atom1;
            AP4_CttsAtom * ctts2 = (AP4_CttsAtom *)atom2;
            AP4_Array<AP4_CttsTableEntry> const & ctts_tbl = ctts2->GetEntries();
            for (AP4_Ordinal i = 0; i < ctts_tbl.ItemCount(); ++i) {
                ctts1->AddEntry(ctts_tbl[i].m_SampleCount, ctts_tbl[i].m_SampleOffset);
            }
            ctts1->GetParent()->OnChildChanged(ctts1);
            return true;
        }

        /* Sample size atom (/moov/trak/minf/stbl/stsz)
        *  .....
        *  Number of entries
        *  Sample size table
        *      Size
        *      Size
        *  .....
        */
        static bool ap4_merge_track_stsz(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StszAtom * stsz1 = (AP4_StszAtom *)atom1;
            AP4_StszAtom * stsz2 = (AP4_StszAtom *)atom2;
            for (AP4_UI32 i = 1; i <= stsz2->GetSampleCount(); i++) {
                AP4_Size sample_size;
                stsz2->GetSampleSize(i, sample_size);
                stsz1->AddEntry(sample_size);
            }
            stsz1->GetParent()->OnChildChanged(stsz1);
            return true;
        }

        /* Sync sample atom (/moov/trak/minf/stbl/stss)
        *  .....
        *  Number of entries
        *  Sync sample table
        *      Number
        *      Number
        *  .....
        */
        static bool ap4_merge_track_stss(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StssAtom * stss1 = (AP4_StssAtom *)atom1;
            AP4_StssAtom * stss2 = (AP4_StssAtom *)atom2;
            AP4_StszAtom * stsz1 = (AP4_StszAtom *)atom1->GetParent()->GetChild(AP4_ATOM_TYPE_STSZ);
            AP4_UI32 sample_count1 = stsz1->GetSampleCount();
            const AP4_Array<AP4_UI32> & stss_tbl2 = stss2->GetEntries();
            for (AP4_Cardinal i = 0; i < stss_tbl2.ItemCount(); i++) {
                stss1->AddEntry(stss_tbl2[i] + sample_count1);
            }
            stss1->GetParent()->OnChildChanged(stss1);
            return true;
        }

        /* Sample-to-chunk atom (/moov/trak/minf/stbl/stsc)
        *  .....
        *  Number of entries
        *  Sample-to-chunk table
        *      Sample-to-chunk table entry
        *          First chunk
        *          Sample per chunk
        *          Sample description ID
        *      Sample-to-chunk table entry
        *          First chunk
        *          Sample per chunk
        *          Sample description ID
        *  .....
        */
        static bool ap4_merge_track_stsc(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StscAtom * stsc1 = (AP4_StscAtom *)atom1;
            AP4_StscAtom * stsc2 = (AP4_StscAtom *)atom2;
            // AP4 记录的最后一个m_ChunkCount为0，表示无穷大，这里需要修正为实际值
            AP4_Array<AP4_StscTableEntry> const & stsc_tbl1 = stsc1->GetEntries();
            if (stsc_tbl1.ItemCount() > 0) {
                AP4_StcoAtom * stco1 = (AP4_StcoAtom *)atom1->GetParent()->GetChild(AP4_ATOM_TYPE_STCO);
                AP4_Cardinal chunk_count1 = stco1->GetChunkCount();
                AP4_StscTableEntry modify_entry = stsc_tbl1[stsc_tbl1.ItemCount() - 1];
                modify_entry.m_ChunkCount = chunk_count1 + 1 - modify_entry.m_FirstChunk;
                stsc1->SetEntry(stsc_tbl1.ItemCount() - 1, modify_entry);
            }
            AP4_Array<AP4_StscTableEntry> const & stsc_tbl2 = stsc2->GetEntries();
            for (AP4_Ordinal i = 0; i < stsc_tbl2.ItemCount(); ++i) {
                stsc1->AddEntry(
                    stsc_tbl2[i].m_ChunkCount,
                    stsc_tbl2[i].m_SamplesPerChunk,
                    stsc_tbl2[i].m_SampleDescriptionIndex);
            }
            stsc1->GetParent()->OnChildChanged(stsc1);
            return true;
        }

        /* Chunk offset atom (/moov/trak/minf/stbl/stco)
        *  .....
        *  Number of entries
        *  Chunk offset table
        *      Offset
        *      Offset
        *  .....
        */

        // 先调整Chunk Offset，减去分段1的头部大小，分段2的Chunk Offset紧跟着分段1的Chunk Offset
        static bool ap4_merge_track_stco_1(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StcoAtom * stco1 = (AP4_StcoAtom *)atom1;
            AP4_StcoAtom * stco2 = (AP4_StcoAtom *)atom2;
            AP4_Atom * mdat1 = file1->GetChild(AP4_ATOM_TYPE_MDAT);
            AP4_Atom * mdat2 = file2->GetChild(AP4_ATOM_TYPE_MDAT);
            AP4_UI64 head_size1 = 0;
            file1->GetChildren().Apply(AP4_AtomSizeAdder(head_size1));
            head_size1 -= mdat1->GetSize() - mdat1->GetHeaderSize();
            AP4_UI64 head_size2 = 0;
            file2->GetChildren().Apply(AP4_AtomSizeAdder(head_size2));
            head_size2 -= mdat2->GetSize() - mdat2->GetHeaderSize();
            stco1->AdjustChunkOffsets(-(int)head_size1);
            stco2->AdjustChunkOffsets((int)(mdat1->GetSize() - mdat1->GetHeaderSize() - head_size2));
            return true;
        }

        // Chunk Offset数值合并
        static bool ap4_merge_track_stco(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StcoAtom * stco1 = (AP4_StcoAtom *)atom1;
            AP4_StcoAtom * stco2 = (AP4_StcoAtom *)atom2;
            AP4_Cardinal chunk_count1 = stco1->GetChunkCount();
            stco1->ReSetEntryCount(stco1->GetChunkCount() + stco2->GetChunkCount());
            memcpy(stco1->GetChunkOffsets() + chunk_count1, stco2->GetChunkOffsets(), stco2->GetChunkCount() * sizeof(AP4_UI32));
            stco1->GetParent()->OnChildChanged(stco1);
            return true;
        }

        // 最后调整Chunk Offset，加上分段1的头部大小
        static bool ap4_merge_track_stco_2(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Atom * atom1, 
            AP4_Atom * atom2, 
            boost::system::error_code & ec)
        {
            AP4_StcoAtom * stco1 = (AP4_StcoAtom *)atom1;
            //AP4_StcoAtom * stco2 = (AP4_StcoAtom *)atom2;
            AP4_UI64 head_size1 = 0;
            AP4_Atom * mdat1 = file1->GetChild(AP4_ATOM_TYPE_MDAT);
            file1->GetChildren().Apply(AP4_AtomSizeAdder(head_size1));
            head_size1 -= mdat1->GetSize() - mdat1->GetHeaderSize();
            stco1->AdjustChunkOffsets((int)head_size1);
            return true;
        }

        static bool ap4_merge_track_1(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Track * track1, 
            AP4_Track * track2, 
            boost::system::error_code & ec)
        {
            AP4_Atom * atom1 = track1->GetTrakAtom()->FindChild("mdia/minf/stbl/stco");
            AP4_Atom * atom2 = track2->GetTrakAtom()->FindChild("mdia/minf/stbl/stco");
            if ((atom1 == NULL || atom2 == NULL)) {
                ec = ap4_error::invalid_mp4_truck;
                return false;
            }
            return ap4_merge_track_stco_1(file1, file2, atom1, atom2, ec);
        }

        static bool ap4_merge_track(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Track * track1, 
            AP4_Track * track2, 
            boost::system::error_code & ec)
        {
            struct {
                char const * path;
                bool must_exist;
                bool (*handler)(
                    AP4_File * file1, 
                    AP4_File * file2, 
                    AP4_Atom * atom1, 
                    AP4_Atom * atom2, 
                    boost::system::error_code & ec);
            } modify_table [] = {
                {"tkhd",                true,  ap4_merge_track_tkhd}, 
                {"mdia/mdhd",           true,  ap4_merge_track_mdhd}, 
                {"mdia/minf/stbl/stts", true,  ap4_merge_track_stts}, 
                {"mdia/minf/stbl/ctts", false, ap4_merge_track_ctts}, 
                {"mdia/minf/stbl/stss", false, ap4_merge_track_stss}, 
                {"mdia/minf/stbl/stsz", true,  ap4_merge_track_stsz}, // stsz 必须在 stss 后面，因为 stss 合并依赖的老的 sample count
                {"mdia/minf/stbl/stsc", true,  ap4_merge_track_stsc}, 
                {"mdia/minf/stbl/stco", true,  ap4_merge_track_stco}, // stco 必须在 stsc 后面，因为 stsc 合并依赖的老的 chunk count
            };

            for (size_t i = 0; i < sizeof(modify_table) / sizeof(modify_table[0]); ++i) {
                AP4_Atom * atom1 = track1->GetTrakAtom()->FindChild(modify_table[i].path);
                AP4_Atom * atom2 = track2->GetTrakAtom()->FindChild(modify_table[i].path);
                if (!modify_table[i].must_exist && atom1 == NULL && atom2 == NULL) {
                    continue;
                }
                if ((atom1 == NULL || atom2 == NULL)) {
                    ec = ap4_error::invalid_mp4_truck;
                    return false;
                }
                if (!modify_table[i].handler(file1, file2, atom1, atom2, ec)) {
                    return false;
                }
            }

            return true;
        }

        static bool ap4_merge_track_2(
            AP4_File * file1, 
            AP4_File * file2, 
            AP4_Track * track1, 
            AP4_Track * track2, 
            boost::system::error_code & ec)
        {
            AP4_Atom * atom1 = track1->GetTrakAtom()->FindChild("mdia/minf/stbl/stco");
            AP4_Atom * atom2 = track2->GetTrakAtom()->FindChild("mdia/minf/stbl/stco");
            if ((atom1 == NULL || atom2 == NULL)) {
                ec = ap4_error::invalid_mp4_truck;
                return false;
            }
            return ap4_merge_track_stco_2(file1, file2, atom1, atom2, ec);
        }

        static bool ap4_merge_two(
            AP4_File * file1, 
            AP4_File * file2, 
            boost::system::error_code & ec)
        {
            // merge mvhd
            AP4_Movie * movie1 = file1->GetMovie();
            AP4_Movie * movie2 = file2->GetMovie();
            if (movie1 == NULL || movie2 == NULL) {
                ec = ap4_error::invalid_mp4_head;
                return false;
            }

            AP4_Atom * mdat1 = file1->GetChild(AP4_ATOM_TYPE_MDAT);
            AP4_Atom * mdat2 = file2->GetChild(AP4_ATOM_TYPE_MDAT);
            if (mdat1 == NULL || mdat2 == NULL) {
                ec = ap4_error::invalid_mp4_head;
                return false;
            }
            if (file1->GetChildren().LastItem()->GetData() != mdat1
                || file2->GetChildren().LastItem()->GetData() != mdat2) {
                ec = ap4_error::invalid_mp4_head;
                return false;
            }

            AP4_MvhdAtom * mvhd1 = (AP4_MvhdAtom*)movie1->GetMvhdAtom();
            AP4_MvhdAtom * mvhd2 = (AP4_MvhdAtom*)movie2->GetMvhdAtom();

            if (mvhd1 == NULL || mvhd2 == NULL) {
                ec = ap4_error::invalid_mp4_truck;
                return false;
            }
            if (!ap4_merge_mvhd(file1, file2, mvhd1, mvhd2, ec)) {
                return false;
            }

            AP4_List<AP4_Track>::Item *item1 = NULL;
            AP4_List<AP4_Track>::Item *item2 = NULL;

            // 调整chunk_offset
            item1 = movie1->GetTracks().FirstItem();
            item2 = movie2->GetTracks().FirstItem();
            while (item1 != NULL && item2 != NULL) {
                AP4_Track * track1 = item1->GetData();
                AP4_Track * track2 = item2->GetData();
                assert(track1 && track2);
                if (!ap4_merge_track_1(file1, file2, track1, track2, ec)) {
                    return false;
                }
                item1 = item1->GetNext();
                item2 = item2->GetNext();
            }

            // merge track
            item1 = movie1->GetTracks().FirstItem();
            item2 = movie2->GetTracks().FirstItem();
            while (item1 != NULL && item2 != NULL) {
                AP4_Track * track1 = item1->GetData();
                AP4_Track * track2 = item2->GetData();
                assert(track1 && track2);
                if (!ap4_merge_track(file1, file2, track1, track2, ec)) {
                    return false;
                }
                item1 = item1->GetNext();
                item2 = item2->GetNext();
            }

            // 调整chunk_offset
            item1 = movie1->GetTracks().FirstItem();
            item2 = movie2->GetTracks().FirstItem();
            while (item1 != NULL && item2 != NULL) {
                AP4_Track * track1 = item1->GetData();
                AP4_Track * track2 = item2->GetData();
                assert(track1 && track2);
                if (!ap4_merge_track_2(file1, file2, track1, track2, ec)) {
                    return false;
                }
                item1 = item1->GetNext();
                item2 = item2->GetNext();
            }

            mdat1->SetSize(mdat1->GetSize() + mdat2->GetSize() - mdat2->GetHeaderSize());

            return true;
        }

        bool ap4_merge(
            AP4_File *& file, 
            AP4_ByteStream & stream, 
            ppbox::data::SegmentInfo const & segment, 
            boost::system::error_code & ec)
        {
            AP4_File * file2 = new AP4_File(
                stream, 
                AP4_DefaultAtomFactory::Instance);

            AP4_Atom * mdat2 = file2->GetChildren().LastItem()->GetData();
            if (mdat2 == NULL 
                || mdat2->GetType() != AP4_ATOM_TYPE_MDAT 
                || mdat2->GetHeaderSize() + segment.size != mdat2->GetSize() + segment.head_size) {
                    ec = ap4_error::invalid_mp4_head;
                    return false;
            }

            if (file == NULL) {
                file = file2;
                return true;
            } else {
                ap4_merge_two(file, file2, ec);
                delete file2;
                return !ec;
            }
        }

        bool ap4_write(
            AP4_File *& file, 
            AP4_ByteStream & stream, 
            boost::system::error_code & ec)
        {
            AP4_List<AP4_Atom>::Item* atom_item = file->GetChildren().FirstItem();
            for (; atom_item; atom_item = atom_item->GetNext()) {
                AP4_Atom* atom = atom_item->GetData();
                atom->Write(stream);
            }

            delete file;
            file = NULL;

            return true;
        }


    } // namespace avformat
} // namespace ppbox
