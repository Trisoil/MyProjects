using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet.Live
{
    using Lucene.Net.Search;

    public class LiveEpgNode
    {
        public static readonly string Name = "name";
        public static readonly string CnName = "cnname";
        public static readonly string ParadeName = "paradename";
        public static readonly string ParadeCnName = "paradecnname";
        public static readonly string Flags = "flags";
        public static readonly string PlatformIndex = "pindex";
        public static readonly string ForbiddenCode = "fbcode";

        public static readonly string IndexValue = "vid";
        public static readonly string ChannelId = "channelid";
        public static readonly string ParadeId = "paradeid";
        public static readonly string VipFlag = "vip";
        public static readonly string TableType = "tabletype";
        public static readonly string IsGroup = "group";

        public static readonly string BitrateRange = "bitrate";
        public static readonly string HightRange = "hight";
        public static readonly string WidthRange = "width";
        public static readonly string ParadeBeginTime = "begintime";

        public static readonly string[] NameFields = new string[] { Name };
        public static readonly string[] CnNameField = new string[] { CnName };
        public static readonly string[] ParadeFields = new string[] { ParadeName };
        public static readonly string[] ParadeCnFields = new string[] { ParadeCnName };

        public static BooleanClause.Occur[] Occurs = new BooleanClause.Occur[] { BooleanClause.Occur.SHOULD };
        
    }
}
