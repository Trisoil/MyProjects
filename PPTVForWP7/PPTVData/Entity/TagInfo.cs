using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class TagInfo
    {
        private int _typeID;
        private string _dimension;
        private string _tagName;
        private int _tagCount;
        public TagInfo() { }
        public TagInfo(int typeID, string dimension, string tagName, int tagCount)
        {
            _typeID = typeID;
            _dimension = dimension;
            _tagName = tagName;
            _tagCount = tagCount;
        }
        public int TypeID { get { return _typeID; } }
        public string Dimension { get { return _dimension; } }
        public string TagName { get { return _tagName; } }
        public int TagCount { get { return _tagCount; } }
    }
}
