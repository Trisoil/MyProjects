using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class RecommandNav
    {
        public RecommandNav()
        { }

        public RecommandNav(int navid, string name, string image)
        {
            _navid = navid;
            _name = name;
            _image = image;
        }

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
    }
}
