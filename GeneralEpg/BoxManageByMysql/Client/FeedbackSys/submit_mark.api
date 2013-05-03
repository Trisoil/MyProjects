<%@ WebHandler Language="C#" Class="submit_mark" %>

using System;
using System.Web;

using Synacast.BoxManage.List.Feedback;
using Synacast.BoxManage.Client;
using Synacast.BoxManage.Client.Help;

public class submit_mark : IHttpHandler {
    
    public void ProcessRequest (HttpContext context) {
        var filter = new FeedbackFilter();
        filter.auth = context.Request["auth"];
        filter.vid = ClientUtils.FormatIntV(context.Request["vid"], "vid");
        filter.mark = ClientUtils.FormatDouble(context.Request["mark"], "mark");
        var feedback = ServiceInvoke<IFeedback>.CreateContract("feedbackservice");
        context.Response.Write(ServiceInvoke<IFeedback>.Invoke<string>(feedback, proxy => proxy.SubmitMark(filter)));
    }
 
    public bool IsReusable {
        get {
            return false;
        }
    }

}