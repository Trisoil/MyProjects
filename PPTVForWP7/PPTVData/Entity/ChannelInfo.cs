using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class ChannelInfo
    {
        private int _vid;
        private string _title;
        private string _playLink;
        private int _typeID;
        private string _imgUrl;
        private string _catalogTags;
        private string _directorTags;
        private string _actTags;
        private string _yearTags;
        private string _areaTags;
        private int _state;
        private string _note;
        private double _mark;
        private int _bitrate;
        private int _resolutionWidth;
        private int _resolutionHeight;
        private bool _isHD;
        private bool _isRecommend;
        private bool _isNew;
        private bool _is3D;
        private double _duration;
        private string _content;
        private string _slotUrl;
        public ChannelInfo() { _vid = -1; }
        public ChannelInfo(
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
            string content,
            string slotUrl
            )
        {
            _vid = vid;
            _title = title;
            _playLink = playLink;
            _typeID = typeID;
            _imgUrl = imgUrl;
            _catalogTags = catalogTags;
            _directorTags = directorTags;
            _actTags = actTags;
            _yearTags = yearTags;
            _areaTags = areaTags;
            _state = state;
            _note = note;
            _mark = mark;
            _bitrate = bitrate;
            _resolutionWidth = resolutionWidth;
            _resolutionHeight = resolutionHeight;
            _isHD = isHD;
            _isRecommend = isRecommend;
            _isNew = isNew;
            _is3D = is3D;
            _duration = duration;
            _content = content;
            _slotUrl = slotUrl;
        }

        public ChannelInfo(
            int vid,
            string title,
            string playLink,
            string imgUrl,
            int bitrate,
            bool isHD,
            bool isRecommend,
            bool isNew,
            bool is3D,
            int resolutionWidth,
            int resolutionHeight,
            int typeID,
            string slotUrl
            )
        {
            _vid = vid;
            _title = title;
            _playLink = playLink;
            _imgUrl = imgUrl;
            _bitrate = bitrate;
            _resolutionWidth = resolutionWidth;
            _resolutionHeight = resolutionHeight;
            _isHD = isHD;
            _isRecommend = isRecommend;
            _isNew = isNew;
            _is3D = is3D;
            _typeID = typeID;
            _slotUrl = slotUrl;
        }

        public int VID { get { return _vid; } }
        public string Title { get { return _title; } }
        public string PlayLink { get { return _playLink; } }
        public int TypeID { get { return _typeID; } }
        public string ImgUrl { get { return _imgUrl; } set { _imgUrl = value; } }
        public string CatalogTags { get { return _catalogTags; } }
        public string DirectorTags { get { return _directorTags; } }
        public string ActTags { get { return _actTags; } }
        public string YearTags { get { return _yearTags; } }
        public string AreaTags { get { return _areaTags; } }
        public int State { get { return _state; } }
        public string Note { get { return _note; } }
        public double Mark { get { return _mark; } }
        public int Bitrate { get { return _bitrate; } }
        public int ResolutionWidth { get { return _resolutionWidth; } }
        public int ResolutionHeight { get { return _resolutionHeight; } }
        public bool IsHD { get { return _isHD; } }
        public bool IsRecommend { get { return _isRecommend; } }
        public bool IsNew { get { return _isNew; } }
        public bool Is3D { get { return _is3D; } }
        public double Duration { get { return _duration; } }
        public string Content { get { return _content; } }
        public string SlotUrl { get { return _slotUrl; } set { _slotUrl = value; } }
        // Content 就是剧情介绍
        public override string ToString()
        {
            return "Channel:{" + _vid + "," + _title + "}";
        }
    }
}
