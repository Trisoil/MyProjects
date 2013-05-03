<%@ WebHandler Language="C#" Class="relevance" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Vod.Relevance;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class relevance : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new RelevanceFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 20, "c");
        filter.vids = ClientUtils.FormatString(context.Request["vids"], "vids");
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        var relevance = ServiceInvoke<IRelevance>.CreateContract("vodrelevance");
        context.Response.Write(ServiceInvoke<IRelevance>.Invoke<string>(relevance, proxy => proxy.Relevance(filter)));
        
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}