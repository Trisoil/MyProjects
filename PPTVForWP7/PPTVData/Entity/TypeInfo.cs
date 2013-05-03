using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class TypeInfo
    {
        private int _typeID;
        private string _typeName;
        public int _typeCount;

        public TypeInfo()
        { }

        public TypeInfo(int typeID, string typeName, int typeCount)
        {
            _typeCount = typeCount;
            _typeID = typeID;
            _typeName = typeName;
        }
        public int TypeID { get { return _typeID; } }
        public string TypeName { get { return _typeName; } }
        /// <summary>
        /// 表示该分类下的影片数
        /// </summary>
        public int TypeCount { get { return _typeCount; } }
    }
}
