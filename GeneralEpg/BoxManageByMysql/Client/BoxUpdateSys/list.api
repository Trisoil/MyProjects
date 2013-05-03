<%@ WebHandler Language="C#" Class="list" %>

using System;
using System.Web;
using System.Xml.Linq;

using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;
using Synacast.BoxManage.List.Update;

public class list : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        UpdateFilter filter = new UpdateFilter();
        filter.c = ClientUtils.FormatIntV(context.Request["c"], 1000, "c");
        filter.s = ClientUtils.FormatIntV(context.Request["s"], "s");
        IUpdate update = ServiceInvoke<IUpdate>.CreateContract("boxupdate");
        context.Response.Write(ServiceInvoke<IUpdate>.Invoke<string>(update, proxy => proxy.AllList(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}