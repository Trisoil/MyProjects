<%@ WebHandler Language="C#" Class="live_recommand_nav_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_recommand_nav_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveNavFilter();
        filter.auth = context.Request["auth"];
        filter.mode = ClientUtils.FormatInt(context.Request["mode"]);
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
        var nav = ServiceInvoke<ILiveNav>.CreateContract("boxlivenav");
        context.Response.Write(ServiceInvoke<ILiveNav>.Invoke<string>(nav, proxy => proxy.LiveRecommandNavEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}