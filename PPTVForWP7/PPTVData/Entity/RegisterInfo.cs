using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVData.Entity
{
    public class RegisterInfo
    {
        public RegisterInfo(string userName, string passWord, string userMail)
        {
            UserName = userName;
            PassWord = passWord;
            UserMail = userMail;
        }
        public RegisterInfo() { }
        public string UserName { get; set; }
        public string PassWord { get; set; }
        public string UserMail { get; set; }
        //public string RandomKey { get; set; }
        //public string ValidateKey { get; set; }
        //public string ReturnURL { get; set; }
    }
}
