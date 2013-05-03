<%@ WebHandler Language="C#" Class="live_parade" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Live;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class live_parade : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new LiveFilter();
        filter.auth = context.Request["auth"];
        filter.vid = ClientUtils.FormatIntV(context.Request["vid"], "vid");
        filter.date = ClientUtils.FormatLiveDate(context.Request["date"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        var live = ServiceInvoke<ILive>.CreateContract("boxlive");
        context.Response.Write(ServiceInvoke<ILive>.Invoke<string>(live, proxy => proxy.LiveParade(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}