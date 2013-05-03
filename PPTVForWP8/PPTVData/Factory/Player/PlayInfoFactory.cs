using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Globalization;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    [Obsolete]
    public class PlayInfoFactory : HttpFactoryBase<Object>
    {
        private bool _isBackThreadCallBack;
        private bool _isDrag = false;

        public PlayInfoFactory()
            : this(false)
        { 
        }

        public PlayInfoFactory(bool isBackThreadCallBack)
            : base()
        {
            _isBackThreadCallBack = isBackThreadCallBack;  
        }

        public Dictionary<int, PlayInfo> PlayInfos { get; set; }

        protected override bool IsBackThreadCallBack
        {
            get
            {
                return _isBackThreadCallBack;
            }
        }

        public bool IsDrag
        {
            get { return _isDrag; }
            set { _isDrag = value; }
        }

        protected override string CreateUri(params object[] paras)
        {
            if (!_isDrag)
                return String.Format("http://play.api.pptv.com/boxplay.api?id={0}&auth={1}&platform={2}", paras[0], Utils.EpgUtils.Auth, Utils.EpgUtils.PlatformName);
            else
                return String.Format("http://play.api.pptv.com/boxplay.api?id={0}&auth={1}&platform={2}&content=need_drag", paras[0], Utils.EpgUtils.Auth, Utils.EpgUtils.PlatformName);
        }

        protected override Object AnalysisData(System.Xml.XmlReader reader)
        {
            PlayInfos = new Dictionary<int, PlayInfo>(6);

            double totalDuraiton = 0;

            if (reader.ReadToFollowing("channel"))
            {
                totalDuraiton = double.Parse(reader["dur"]);
                while (reader.Read())
                {
                    if (reader.Name == "file"
                         && reader.NodeType == XmlNodeType.Element)
                    {
                        reader.Read();
                        while (reader.IsStartElement("item"))
                        {
                            var info = new PlayInfo();
                            if (IsDrag) info.Segments = new List<PlaySegmentInfo>();
                            var bitrate = reader.GetAttribute("bitrate");
                            var ft = reader.GetAttribute("ft");
                            if (bitrate == null || ft == null)
                                continue;
                            info.Bitrate = Convert.ToInt32(bitrate);
                            info.ft = Convert.ToInt32(ft);
                            info.Rid = reader.GetAttribute("rid");
                            info.TotalDuration = totalDuraiton;
                            PlayInfos.Add(info.ft, info);
                            reader.ReadToNextSibling("item");
                        }
                    }

                    if (reader.Name == "dt" 
                        && reader.NodeType == XmlNodeType.Element)
                    {
                        var ft = Convert.ToInt32(reader.GetAttribute("ft"));
                        if (PlayInfos.ContainsKey(ft))
                        {
                            var info = PlayInfos[ft];
                            reader.ReadToFollowing("sh");
                            reader.Read();
                            info.Sh = reader.Value;
                            reader.ReadToFollowing("st");
                            reader.Read();
                            var st = reader.Value;
                            var ci = new CultureInfo("en-US");
                            info.St = DateTime.ParseExact(st, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                            reader.ReadToFollowing("bwt");
                            reader.Read();
                            info.Bwt = Convert.ToInt32(reader.Value);
                        }
                    }

                    if (reader.Name == "drag"
                        && reader.NodeType == XmlNodeType.Element)
                    {
                        var ft = Convert.ToInt32(reader.GetAttribute("ft"));
                        if (PlayInfos.ContainsKey(ft))
                        {
                            reader.Read();
                            var info = PlayInfos[ft];
                            double timestamp = 0;
                            while (reader.IsStartElement("sgm"))
                            {
                                var segment = new PlaySegmentInfo();
                                segment.No = int.Parse(reader["no"]);
                                segment.Duration = double.Parse(reader["dur"]);
                                segment.FileSize = ulong.Parse(reader["fs"]);
                                segment.HeadLength = ulong.Parse(reader["hl"]); 
                                timestamp += segment.Duration;
                                segment.TotalDuration = timestamp;
                                info.Segments.Add(segment);
                                reader.ReadToNextSibling("sgm");
                            }
                        }
                    }
                }
            }

            return 1;
        }

        public PlayInfo CreatePlayInfo(PlayFileType ft = PlayFileType.HD)
        {
            if (PlayInfos != null && PlayInfos.Count > 0)
            {
                var ftype = (int)ft;
                while (!PlayInfos.ContainsKey(ftype) && ftype >= 0)
                {
                    ftype--;
                }

                if (ftype < 0)
                {
                    return PlayInfos.Values.First();
                }

                if (PlayInfos.ContainsKey(ftype))
                {
                    return PlayInfos[ftype];
                }
            }
            return null;
        }

        public string CreatePlayUri(int ft)
        {
            if (PlayInfos != null)
            {
                var info = PlayInfos[ft];
                var key = KeyGenerator.GetKey(info.St);
                return string.Format("http://{0}:80/{1}?type={2}&w=1&key={3}", info.Sh, info.Rid, Utils.EpgUtils.PlatformName, key);
            }
            return string.Empty;
        }

        public string CreatePlayUri(PlayFileType ft = PlayFileType.HD)
        {
            if (PlayInfos != null && PlayInfos.Count > 0)
            {
                var ftype = (int)ft;
                while (!PlayInfos.ContainsKey(ftype) && ftype >= 0)
                {
                    ftype--;
                }

                PlayInfo info = null;
                if (ftype < 0)
                    info = PlayInfos.Values.First();
                else if (PlayInfos.ContainsKey(ftype))
                    info = PlayInfos[ftype];

                if (info != null)
                {
                    var key = KeyGenerator.GetKey(info.St);
                    return string.Format("http://{0}:80/{1}?type={2}&w=1&key={3}", info.Sh, info.Rid, Utils.EpgUtils.PlatformName, key);
                }
            }
            return string.Empty;
        }
    }
}
