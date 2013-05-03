using System;
using System.Net;

namespace PPTVData.Entity
{
    public class UserInfo
    {
        public UserInfo() { }
        public UserInfo(string userName, string passWord, string id)
        {
            UserName = userName;
            PassWord = passWord;
            ID = id;
        }
        public string UserName { get; set; }
        public string PassWord { get; set; }
        public string ID { get; set; }
    }
}
