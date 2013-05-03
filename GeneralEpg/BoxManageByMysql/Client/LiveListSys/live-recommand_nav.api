<%@ WebHandler Language="C#" Class="live_recommand_nav" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live.Nav;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_recommand_nav : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveNavFilter();
        filter.auth = context.Request["auth"];
        filter.mode = ClientUtils.FormatInt(context.Request["mode"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var nav = ServiceInvoke<ILiveNav>.CreateContract("boxlivenav");
        context.Response.Write(ServiceInvoke<ILiveNav>.Invoke<string>(nav, proxy => proxy.LiveRecommandNav(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}