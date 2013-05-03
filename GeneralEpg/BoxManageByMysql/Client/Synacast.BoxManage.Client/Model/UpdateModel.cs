using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;

namespace Synacast.BoxManage.Client.Model
{
    using log4net;
    using Synacast.BoxManage.Client.Help;

    public class UpdateModel:IHttpModule
    {
        #region Fields

        static ILog logger = ClientUtils.GetLogger("App.log");

        #endregion

        public string Name
        {
            get { return "UpdateListModel"; }
        }

        #region IHttpModule 成员

        public void Dispose()
        {
            throw new NotImplementedException();
        }

        public void Init(HttpApplication context)
        {
            context.BeginRequest += new EventHandler(context_BeginRequest);
            context.Error += new EventHandler(context_Error);
        }

        #endregion

        private void context_BeginRequest(object sender, EventArgs e)
        {
            HttpApplication application = sender as HttpApplication;
            application.Context.Response.ContentType = "text/xml";
            application.Context.Response.Write("<?xml version='1.0' encoding='utf-8' ?>");
        }

        private void context_Error(object sender, EventArgs e)
        {
            HttpApplication application = sender as HttpApplication;
            Exception ex = application.Context.Server.GetLastError();
            logger.Error(ex);
            application.Context.Response.Write(ClientUtils.FormatErrorMsg(ex.Message));
            application.Context.Server.ClearError();
        }
    }
}
