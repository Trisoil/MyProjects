<%@ WebHandler Language="C#" Class="live_types" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_types : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveFilter();
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var live = ServiceInvoke<ILive>.CreateContract("boxlive");
        context.Response.Write(ServiceInvoke<ILive>.Invoke<string>(live, proxy => proxy.LiveTypes(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}