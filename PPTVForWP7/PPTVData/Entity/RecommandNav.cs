using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class RecommendNav
    {
        private int _navid;
        public int Navid
        {
            get
            {
                return _navid;
            }
        }
        private string _name;
        public string Name
        {
            get
            {
                return _name;
            }
        }
        private string _image;
        public string Image
        {
            get
            {
                return _image;
            }
        }
        private bool _isFromDataBase;
        public bool IsFromDataBase
        {
            get
            {
                return _isFromDataBase;
            }
        }

        public RecommendNav()
        { }
        public RecommendNav(int navid, string name, string image, bool isFromDataBase)
        {
            _navid = navid;
            _name = name;
            _image = image;
            _isFromDataBase = isFromDataBase;
        }
    }
}
