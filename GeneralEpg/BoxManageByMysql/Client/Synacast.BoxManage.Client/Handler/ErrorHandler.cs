using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web.UI;

namespace Synacast.BoxManage.Client.Handler
{
    using Synacast.BoxManage.Client.Help;

    public class ErrorHandler :  Page 
    {
        private string _msg;

        public ErrorHandler(string msg)
        {
            _msg = msg;    
        }

        public override void ProcessRequest(System.Web.HttpContext context)
        {
            //context.Response.ContentType = "text/xml";
            //context.Response.Write("<?xml version='1.0' encoding='utf-8' ?>");
            context.Response.Write(ClientUtils.FormatErrorMsg(_msg));
        }
    }
}
