<%@ WebHandler Language="C#" Class="detail_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Details;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class detail_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new DetailFilter();
        filter.c = ClientUtils.FormatInt(context.Request["c"], 200);
        filter.s = ClientUtils.FormatInt(context.Request["s"]);
        filter.auth = context.Request["auth"];
        filter.vid = ClientUtils.FormatIntV(context.Request["vid"], "vid");
        filter.order = context.Request["order"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);

        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var deta = ServiceInvoke<IDetails>.CreateContract("boxdetail");
        context.Response.Write(ServiceInvoke<IDetails>.Invoke<string>(deta, proxy => proxy.DetailsListEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}