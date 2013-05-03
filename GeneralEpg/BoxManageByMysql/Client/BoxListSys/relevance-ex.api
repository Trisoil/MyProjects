<%@ WebHandler Language="C#" Class="relevance_ex" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Vod.Relevance;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class relevance_ex : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new RelevanceFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 20, "c");
        filter.vids = ClientUtils.FormatString(context.Request["vids"], "vids");
        filter.auth = context.Request["auth"];
        filter.platform = context.Request["platform"];
        filter.conlen = ClientUtils.FormatInt(context.Request["conlen"]);
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);

        filter.forbidvip = ClientUtils.FormatInt(context.Request["forbidvip"]);
        filter.bitratemax = ClientUtils.FormatInt(context.Request["bitrate-max"]);
        filter.bitratemin = ClientUtils.FormatInt(context.Request["bitrate-min"]);
        filter.hmax = ClientUtils.FormatInt(context.Request["h-max"]);
        filter.hmin = ClientUtils.FormatInt(context.Request["h-min"]);
        filter.wmax = ClientUtils.FormatInt(context.Request["w-max"]);
        filter.wmin = ClientUtils.FormatInt(context.Request["w-min"]);
        var relevance = ServiceInvoke<IRelevance>.CreateContract("vodrelevance");
        context.Response.Write(ServiceInvoke<IRelevance>.Invoke<string>(relevance, proxy => proxy.RelevanceEx(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}