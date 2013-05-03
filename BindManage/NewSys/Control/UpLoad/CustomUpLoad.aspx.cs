using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

using RmtInterface;
using RmtLibraryAgent;
using NewWorkFlowSys.HistoryRecord;
using Synacast.PPInstall;


public partial class Control_UpLoad_CustomUpLoad : System.Web.UI.Page
{
    string message = "";
    bool flag = false;

    protected void Page_Load(object sender, EventArgs e)
    {
        try
        {
            string customId = Request["customid"];
            if (!string.IsNullOrEmpty(customId) && Request.Files.Count > 0)
            {
                HttpPostedFile upFile = Request.Files[0];
                if (upFile.InputStream.Length > 0)
                {
                    string name = upFile.FileName.Substring(upFile.FileName.LastIndexOf("\\") + 1);
                    byte[] buffer = new byte[upFile.InputStream.Length];
                    upFile.InputStream.Read(buffer, 0, (int)upFile.InputStream.Length);
                    IRemotingFile ro = RemotingManager.GetManager().GetObject<IRemotingFile>(out message);
                    flag = ro.WriteFileBytes(name, "1", customId, buffer, out message);
                    if (flag)
                    {
                        string eventLog = string.Format("{0}文件名为{1}", message, name);
                        HistoryRecordManage _hr = new HistoryRecordManage();
                        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, "定制信息管理", eventLog);
                    }
                }
            }
        }
        catch (Exception ex)
        {
            message = ex.Message;
        }
    }

    protected override void Render(HtmlTextWriter writer)
    {
        Response.Clear();
        if (flag)//"".Equals(message))
        {
            Response.Write("{success:true}");
        }
        else
        {
            Response.Write("{success:false,message:'" + message + "'}");
        }
        message = "";
        flag = false;
    }
}
