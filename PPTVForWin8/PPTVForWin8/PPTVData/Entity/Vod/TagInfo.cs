using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class TagInfo
    {
        public string Name { get; set; }

        public int Count { get; set; }
    }

    public enum TagDimension
    { 
        Catalog,
        Area,
        Year
    }
}
