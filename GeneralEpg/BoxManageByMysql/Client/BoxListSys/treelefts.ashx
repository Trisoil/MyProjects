<%@ WebHandler Language="C#" Class="treelefts" %>

using System;
using System.Web;

using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;
using Synacast.BoxManage.List.Vod.Treeleft;

public class treelefts : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new TreeleftFilter();
        filter.auth = context.Request["auth"];
        filter.type = ClientUtils.FormatType(context.Request["type"]);
        filter.rank = ClientUtils.FormatDefaultInt(context.Request["rank"], 1);
        filter.treeleftid = ClientUtils.FormatInt(context.Request["treeleftid"]);
        filter.platform = context.Request["platform"];
        filter.lang = ClientUtils.FormatLanguage(context.Request["lang"]);
        filter.ver = ClientUtils.FormatDefaultInt(context.Request["ver"], 2);
        var tree = ServiceInvoke<ITreeleft>.CreateContract("treeleft");
        context.Response.Write(ServiceInvoke<ITreeleft>.Invoke<string>(tree, proxy => proxy.Treelefts(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}