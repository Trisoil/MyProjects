using System;

namespace PPTVData.Entity
{
    public class MainNav
    {
        public string Navid { get; private set; }
        public string NavTitle { get; private set; }
        public string ImageUrl { get; private set; }
        public MainNav(string navid, string navTitle, string imageUrl)
        {
            Navid = navid;
            NavTitle = navTitle;
            ImageUrl = imageUrl;
        }
    }
}
