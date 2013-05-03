// MkvObjectType.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_OBJECT_TYPE_H_
#define _PPBOX_AVFORMAT_MKV_MKV_OBJECT_TYPE_H_

#include "ppbox/avformat/mkv/MkvFormat.h"
#include "ppbox/avformat/mkv/ebml/EBML_File.h"
#include "ppbox/avformat/mkv/MkvSegment.h"

namespace ppbox
{
    namespace avformat
    {

        typedef EBML_OArchive MkvOArchive;

        typedef EBML_IArchive MkvIArchive;

        struct MkvFile
            : EBML_File
        {
            MkvFile()
            {
                DocType = "matroska";
                DocTypeVersion = 2;
                DocTypeReadVersion = 2;
            }
        };

    } // namespace avformat
} // namespace ppbox


#endif // _PPBOX_AVFORMAT_MKV_MKV_OBJECT_TYPE_H_
