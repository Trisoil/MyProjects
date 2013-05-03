using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class HistoryRecordInfo
    {
        public HistoryRecordInfo() { }

        public HistoryRecordInfo(
            int vid,
            string title,
            string playLink,
            int typeID,
            string imgUrl,
            string catalogTags,
            string directorTags,
            string actTags,
            string yearTags,
            string areaTags,
            int state,
            string note,
            double mark,
            int bitrate,
            int resolutionWidth,
            int resolutionHeight,
            bool isHD,
            bool isRecommend,
            bool isNew,
            bool is3D,
            double duration,
            string programTitle,
            double currentPosition,
            int programIndex,
            string content,
            bool isOver
            )
        {
            VID = vid;
            Title = title;
            PlayLink = playLink;
            TypeID = typeID;
            ImgUrl = imgUrl;
            CatalogTags = catalogTags;
            DirectorTags = directorTags;
            ActTags = actTags;
            YearTags = yearTags;
            AreaTags = areaTags;
            State = state;
            Note = note;
            Mark = mark;
            Bitrate = bitrate;
            ResolutionWidth = resolutionWidth;
            ResolutionHeight = resolutionHeight;
            IsHD = isHD;
            IsRecommend = isRecommend;
            IsNew = isNew;
            Is3D = is3D;
            Duration = duration;
            Content = content;
            ProgramTitle = programTitle;
            CurrentPosition = currentPosition;
            ProgramIndex = programIndex;
            IsOver = isOver;
        }
        public int VID { get; set; }
        public string Title { get; set; }
        public string PlayLink { get; set; }
        public int TypeID { get; set; }
        public string ImgUrl { get; set; }
        public string CatalogTags { get; set; }
        public string DirectorTags { get; set; }
        public string ActTags { get; set; }
        public string YearTags { get; set; }
        public string AreaTags { get; set; }
        public int State { get; set; }
        public string Note { get; set; }
        public double Mark { get; set; }
        public int Bitrate { get; set; }
        public int ResolutionWidth { get; set; }
        public int ResolutionHeight { get; set; }
        public bool IsHD { get; set; }
        public bool IsRecommend { get; set; }
        public bool IsNew { get; set; }
        public bool Is3D { get; set; }
        public double Duration { get; set; }
        public string Content { get; set; }
        public string ProgramTitle { get; set; }
        public double CurrentPosition { get; set; }
        public int ProgramIndex { get; set; }
        public bool IsOver { get; set; }
    }
}
