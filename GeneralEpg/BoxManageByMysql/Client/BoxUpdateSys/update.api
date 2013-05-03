<%@ WebHandler Language="C#" Class="today_update" %>

using System;
using System.Web;

using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;
using Synacast.BoxManage.List.Update;

public class today_update : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        UpdateFilter filter = new UpdateFilter();
        //filter.c = ClientUtils.FormatInt(context.Request["c"], 1000);
        //filter.s = ClientUtils.FormatInt(context.Request["s"]);
        filter.lasttime = ClientUtils.FormatDateTime(context.Request["lasttime"], "lasttime");
        IUpdate update = ServiceInvoke<IUpdate>.CreateContract("boxupdate");
        context.Response.Write(ServiceInvoke<IUpdate>.Invoke<string>(update, proxy => proxy.TodayUpdate(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}