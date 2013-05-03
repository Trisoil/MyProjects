using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Globalization;

namespace PPTVData.Factory
{
    using PPTVData.Entity;
    using PPTVData.Utils;

    public class Play2InfoFactory : HttpFactoryBase<Object>
    {
        private string _vvid;
        private string _playType;
        private bool _isBackThreadCallBack;
        private bool _isDownload;

        public Play2InfoFactory()
            : this(false, false)
        { 
        }

        public Play2InfoFactory(bool isBackThreadCallBack, bool isDownload)
            : base()
        {
            _isBackThreadCallBack = isBackThreadCallBack;
            _isDownload = isDownload;
        }

        public Dictionary<int, PlayInfo> PlayInfos { get; set; }

        protected override bool IsBackThreadCallBack
        {
            get
            {
                return _isBackThreadCallBack;
            }
        }

        protected override string CreateUri(params object[] paras)
        {
            if (paras.Length > 1)
                _vvid = paras[1].ToString();
            else
                _vvid = Guid.NewGuid().ToString();

            _playType = EpgUtils.CreatePlayType(_isDownload);
            return string.Format("http://play.api.pptv.com/boxplay.api?id={0}&vvid={1}&type={2}&auth={3}&platform={4}&gslbversion=2", paras[0], _vvid, _playType, EpgUtils.Auth, EpgUtils.PlatformName);
        }

        protected override object AnalysisData(System.Xml.XmlReader reader)
        {
            PlayInfos = new Dictionary<int, PlayInfo>(6);
            var ci = new CultureInfo("en-US");

            while (reader.Read())
            {
                if (reader.Name == "channel"
                    && reader.NodeType == XmlNodeType.Element)
                {
                    var totalDuraiton = double.Parse(reader["dur"]);

                    while (reader.Read())
                    {
                        if (reader.Name == "file"
                             && reader.NodeType == XmlNodeType.Element)
                        {
                            reader.Read();
                            while (reader.IsStartElement("item"))
                            {
                                var info = new PlayInfo();
                                var bitrate = reader["bitrate"];
                                var ft = reader["ft"];
                                if (bitrate == null || ft == null)
                                    continue;
                                info.Bitrate = Convert.ToInt32(bitrate);
                                info.ft = Convert.ToInt32(ft);
                                info.Rid = reader["rid"];
                                info.TotalDuration = totalDuraiton;
                                PlayInfos.Add(info.ft, info);
                                reader.ReadToNextSibling("item");
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement
                            && reader.Name == "file")
                            break;
                    }
                }

                if (reader.Name == "dt"
                    && reader.NodeType == XmlNodeType.Element)
                {
                    var ft = Convert.ToInt32(reader["ft"]);
                    reader.Read();
                    if (!PlayInfos.ContainsKey(ft)) continue;

                    var info = PlayInfos[ft];
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element 
                            && !reader.IsEmptyElement)
                        {
                            var nodeName = reader.Name;
                            if (nodeName != "key")
                                reader.Read();
                            switch (nodeName)
                            {
                                case "sh":
                                    info.Sh = reader.Value;
                                    break;
                                case "st":
                                    info.St = DateTime.ParseExact(reader.Value, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                                    break;
                                case "bwt":
                                    info.Bwt = Convert.ToInt32(reader.Value);
                                    break;
                                case "key":
                                    info.Expire = DateTime.ParseExact(reader["expire"], "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                                    reader.Read();
                                    info.Key = reader.Value;
                                    break;
                                default:
                                    break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement
                                && reader.Name == "dt")
                            break;
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
                return GetPlayUri(info);
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
                    return GetPlayUri(info);
            }
            return string.Empty;
        }

        private string GetPlayUri(PlayInfo playInfo)
        {
            string key = string.Empty;
            if (!string.IsNullOrEmpty(playInfo.Key))
                key = string.Format("k={0}", playInfo.Key);
            else
                key = string.Format("key={0}", KeyGenerator.GetKey(playInfo.St));

            return string.Format("http://{0}/{1}?type={2}&sv={3}&vvid={4}&{5}&w=1",
                playInfo.Sh, playInfo.Rid, _playType, EpgUtils.ClientVersion, _vvid, key);
        }
    }
}
