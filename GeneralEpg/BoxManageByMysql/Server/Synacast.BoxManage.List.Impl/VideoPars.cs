using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl
{
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.Core.Cache.Entity;

    /// <summary>
    /// List请求参数对象
    /// </summary>
    public class VideoPars
    {
        //public List<string> Catalogs;

        //public List<string> Areas;

        //public List<string> Years;

        //public List<string> Acts;

        //public List<string> Directors;

        public IEnumerable<string> Tags;

        public string[] Flags;

        public string SortTag;

        public string ForbiddenName;

        public Func<VideoNode, ExFilterBase, bool, XElement> ResponseFun;
        
    }
}
