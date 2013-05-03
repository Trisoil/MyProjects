<%@ WebHandler Language="C#" Class="blueray_nav_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class blueray_nav_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        NavFilter filter = new NavFilter();
        filter.auth = context.Request["auth"];
        filter.mode = -1;
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        
        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        INav nav = ServiceInvoke<INav>.CreateContract("boxnav");
        context.Response.Write(ServiceInvoke<INav>.Invoke<string>(nav, proxy => proxy.NavEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}