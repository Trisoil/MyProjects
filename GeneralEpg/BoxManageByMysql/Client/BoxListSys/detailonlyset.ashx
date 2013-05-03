<%@ WebHandler Language="C#" Class="detailonlyset" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Details;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class detailonlyset : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        DetailFilter filter = new DetailFilter();
        filter.c = ClientUtils.FormatInt(context.Request["c"], 200);
        filter.s = ClientUtils.FormatInt(context.Request["s"]);
        filter.auth = context.Request["auth"];
        filter.order = context.Request["order"];
        filter.vid = ClientUtils.FormatIntV(context.Request["vid"], "vid");
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 1);
        IDetails deta = ServiceInvoke<IDetails>.CreateContract("boxdetail");
        context.Response.Write(ServiceInvoke<IDetails>.Invoke<string>(deta, proxy => proxy.DetailsOnlySet(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}