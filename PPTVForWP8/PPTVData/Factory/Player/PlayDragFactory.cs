using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading.Tasks;

namespace PPTVData.Factory.Player
{
    using PPTVData.Entity.Player;

    public class PlayDragFactory : HttpFactoryBase<PlayDragInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("http://drag.g1d.net/{0}0drag", paras[0]);
        }

        protected override PlayDragInfo AnalysisData(XmlReader reader)
        {
            var dragInfo = new PlayDragInfo();

            while (reader.Read())
            { 
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "video")
                {
                    if (reader.ReadToFollowing("filesize"))
                        dragInfo.FileSize = (ulong)reader.ReadElementContentAsLong();
                    if (reader.IsStartElement("duration"))
                        dragInfo.Duration = reader.ReadElementContentAsDouble();
                    if (reader.ReadToFollowing("width"))
                        dragInfo.Width = (uint)reader.ReadElementContentAsInt();
                    if (reader.IsStartElement("height"))
                        dragInfo.Height = (uint)reader.ReadElementContentAsInt();
                }

                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "segments")
                {
                    reader.Read();
                    double timestamp = 0;
                    while (reader.IsStartElement("segment"))
                    {
                        var seginfo = new SegmentInfo();
                        seginfo.No = uint.Parse(reader["no"]);
                        seginfo.Offset = ulong.Parse(reader["offset"]);
                        seginfo.HeadLength = ulong.Parse(reader["headlength"]);
                        seginfo.Duration = double.Parse(reader["duration"]);
                        seginfo.Varid = reader["varid"];
                        seginfo.FileSize = ulong.Parse(reader["filesize"]);
                        seginfo.TimeStamp = timestamp;
                        timestamp += seginfo.Duration;
                        dragInfo.Segments.Add(seginfo);
                        reader.ReadToNextSibling("segment");
                    }
                }

                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "drag")
                {
                    var infos = reader.ReadElementContentAsString();
                    var drags = infos.Split('|');
                    foreach (var drag in drags)
                    { 
                        var ds = drag.Split(':');
                        if (ds.Length != 3)
                            continue;
                        var d = new DragInfo();
                        d.TimeStamp = double.Parse(ds[0]);
                        d.No = uint.Parse(ds[1]);
                        d.Offset = ulong.Parse(ds[2]);
                        dragInfo.Drags.Add(d);
                    }
                }
            }

            return dragInfo;
        }
    }
}
