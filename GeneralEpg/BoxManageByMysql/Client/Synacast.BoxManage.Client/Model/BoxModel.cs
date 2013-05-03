using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Reflection;
using System.Xml.Linq;
using System.IO;

namespace Synacast.BoxManage.Client.Model
{
    using log4net;
    using Synacast.BoxManage.Client.Help;

    public class BoxModel:IHttpModule 
    {
        private static bool _inited = false;
        private static readonly object _lock = new object();
        private static Dictionary<string, int> _cache = new Dictionary<string, int>(100);
        private static Dictionary<string, bool> _host = new Dictionary<string, bool>(10);
        private static ILog logger = ClientUtils.GetLogger("App.log");

        public string Name
        {
            get { return "BoxCoreModel"; }
        }

        #region IHttpModule 成员

        public void Dispose()
        {
            
        }

        public void Init(HttpApplication context)
        {
            if (!_inited)
            {
                lock (_lock)
                {
                    if (!_inited)
                    {
                        InitDictionary();
                        InitHost();
                        _inited = true;
                    }
                }
            }
            context.Error += new EventHandler(context_Error);
            context.BeginRequest += new EventHandler(context_BeginRequest);
            context.EndRequest += new EventHandler(context_EndRequest);
        }

        #endregion

        private void context_Error(object sender, EventArgs e)
        {
            HttpApplication application = sender as HttpApplication;
            Exception ex = application.Context.Server.GetLastError();
            logger.Error(ex);
            application.CompleteRequest();
            application.Context.Response.Write(ClientUtils.FormatErrorMsg(ex.Message));
            //application.Context.Server.ClearError();
        }

        private void context_BeginRequest(object sender, EventArgs e)
        {
            HttpApplication application = sender as HttpApplication;
            application.Context.Response.ContentType = "text/xml";
            application.Context.Response.Write("<?xml version='1.0' encoding='utf-8' ?>");
            string auth = application.Context.Request["auth"];
            if (string.IsNullOrEmpty(auth))
            {
                application.CompleteRequest();
                application.Context.Response.Write(ClientUtils.FormatErrorMsg("没有权限查看"));
            }
            var host = application.Context.Request["SERVER_NAME"];
            var forbidden = true;
            if (_host.ContainsKey(host))
            {
                forbidden = _host[host];
            }
            if (forbidden)
            {
                var fb = application.Context.Request["fb"];
                if (fb != null && _host.ContainsKey(fb))
                {
                    forbidden = _host[fb];
                }
            }
            application.Context.Items.Add("forbidden", forbidden);
        }

        private void context_EndRequest(object sender, EventArgs e)
        {
            var application = sender as HttpApplication;
            var context = application.Context;
            var filepath = context.Request.FilePath;
            var filename = Path.GetFileNameWithoutExtension(VirtualPathUtility.GetFileName(filepath));
            if (application.Context.Server.GetLastError() == null && _cache.ContainsKey(filename))
            {
                var value = _cache[filename];
                context.Response.Expires = value;
                context.Response.Cache.SetCacheability(HttpCacheability.Public);
                context.Response.Cache.SetMaxAge(TimeSpan.FromMinutes(value));
            }
            else
            {
                application.Context.Server.ClearError();
                context.Response.Expires = 1;
                context.Response.Cache.SetCacheability(HttpCacheability.Public);
                context.Response.Cache.SetMaxAge(TimeSpan.FromMinutes(1));
            }
        }

        private void InitDictionary()
        {
            var assembly = Assembly.GetCallingAssembly();
            var stream = assembly.GetManifestResourceStream("Synacast.BoxManage.Client.Cache.xml");
            if (stream != null)
            {
                using (var reader = new System.Xml.XmlTextReader(stream))
                {
                    var xml = XElement.Load(reader);
                    var query = from face in xml.Elements("Interface") select new KeyValuePair<string, int>(face.Element("Name").Value, int.Parse(face.Element("Time").Value));
                    foreach (var face in query)
                    {
                        _cache.Add(face.Key, face.Value);
                    }
                }
            }
        }

        private void InitHost()
        {
            var assembly = Assembly.GetCallingAssembly();
            var stream = assembly.GetManifestResourceStream("Synacast.BoxManage.Client.Host.xml");
            if (stream != null)
            {
                using (var reader = new System.Xml.XmlTextReader(stream))
                {
                    var xml = XElement.Load(reader);
                    var query = from host in xml.Elements("Host") select new KeyValuePair<string, bool>(host.Element("Name").Value, bool.Parse(host.Element("BD").Value));
                    foreach (var host in query)
                    {
                        _host.Add(host.Key, host.Value);
                    }
                }
            }
        }
    }
}
