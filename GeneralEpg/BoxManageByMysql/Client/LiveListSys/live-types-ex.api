<%@ WebHandler Language="C#" Class="live_types_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_types_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveFilter();
        filter.auth = context.Request["auth"];
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
        var live = ServiceInvoke<ILive>.CreateContract("boxlive");
        context.Response.Write(ServiceInvoke<ILive>.Invoke<string>(live, proxy => proxy.LiveTypesEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}