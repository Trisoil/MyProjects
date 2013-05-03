<%@ WebHandler Language="C#" Class="detailwithoutset_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Details;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class detailwithoutset_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        DetailFilter filter = new DetailFilter();
        filter.c = ClientUtils.FormatInt(context.Request["c"], 200);
        filter.s = ClientUtils.FormatInt(context.Request["s"]);
        filter.auth = context.Request["auth"];
        filter.vid = ClientUtils.FormatIntV(context.Request["vid"], "vid");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);

        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        IDetails deta = ServiceInvoke<IDetails>.CreateContract("boxdetail");
        context.Response.Write(ServiceInvoke<IDetails>.Invoke<string>(deta, proxy => proxy.DetailsWithoutSetEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}