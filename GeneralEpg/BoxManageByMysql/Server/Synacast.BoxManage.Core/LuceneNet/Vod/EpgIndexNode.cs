using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.LuceneNet
{
    using Lucene.Net.Search;

    public class EpgIndexNode
    {
        public static readonly string Name = "name";
        public static readonly string CnName = "cnname";
        public static readonly string Catalog = "catalog";
        public static readonly string Area = "area";
        public static readonly string Year = "year";
        public static readonly string Act = "act";
        public static readonly string Director = "director";
        public static readonly string Flags = "flags";
        public static readonly string PlatformIndex = "pindex";
        public static readonly string ForbiddenCode = "fbcode";

        public static readonly string IndexValue = "vid";
        public static readonly string VipFlag = "vip";
        public static readonly string TableType = "tabletype";
        public static readonly string Screen = "screen";
        public static readonly string IsGroup = "group";
        public static readonly string Type = "type";

        public static readonly string BitrateRange = "bitrate";
        public static readonly string HightRange = "hight";
        public static readonly string WidthRange = "width";

        public static readonly string SortFields = "name";

        public static readonly string[] NameField = new string[] { Name };
        public static readonly string[] CnNameField = new string[] { CnName };

        public static readonly BooleanClause.Occur[] NameOccurs = new BooleanClause.Occur[] { BooleanClause.Occur.SHOULD };

        public static readonly string[] Fields = new string[] { Name, Catalog, Area, Year, Act, Director };

        public static BooleanClause.Occur[] Occurs = new BooleanClause.Occur[] { BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD };

        public static readonly string[] RelevanceFields = new string[] { Name };  //, Catalog, Act, Area

        public static BooleanClause.Occur[] RelevanceOccurs = new BooleanClause.Occur[] { BooleanClause.Occur.SHOULD };  //, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD, BooleanClause.Occur.SHOULD
    }
}
