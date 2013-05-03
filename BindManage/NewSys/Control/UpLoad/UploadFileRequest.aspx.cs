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
using System.IO;

using RmtInterface;
using RmtLibraryAgent;
using NewWorkFlowSys.HistoryRecord;
using Synacast.PPInstall;


public partial class Control_UpLoad_UploadFileRequest : System.Web.UI.Page
{
    string message = "";
    bool flag = false;

    protected void Page_Load(object sender, EventArgs e)
    {
        string jSONString = string.Empty;
        try
        {
            string customId = Request["customid"];
            string userName = Request["username"];
            if (!string.IsNullOrEmpty(customId) && Request.Files.Count > 0)
            {
                HttpPostedFile upFile = Request.Files["Filedata"];
                if (upFile.InputStream.Length > 0)
                {
                    string name = upFile.FileName.Substring(upFile.FileName.LastIndexOf("\\") + 1);
                    byte[] buffer = new byte[upFile.InputStream.Length];
                    upFile.InputStream.Read(buffer, 0, (int)upFile.InputStream.Length);
                    IRemotingFile ro = RemotingManager.GetManager().GetObject<IRemotingFile>(out message);
                    flag = ro.WriteFileBytes(name,Request["bagtype"] , customId, buffer, out message);
                    if (flag)
                    {
                        string eventLog = string.Format("{0}文件名为{1}", message, name);
                        HistoryRecordManage _hr = new HistoryRecordManage();
                        _hr.SaveEventLog(userName, PPInstallHelp.ClientIp, "定制信息管理", eventLog);
                    }
                }

                //jSONString = "{success:true,message:'上传成功'}";
                message = "上传成功！";
                Response.StatusCode = 200;
            }
        }
        catch (System.Runtime.Remoting.RemotingException ex)
        {
            Response.StatusCode = 405;
            message = ex.Message;
        }
        catch (System.Net.Sockets.SocketException ex)
        {
            Response.StatusCode = 405;
            message = ex.Message;
        }
        catch (Exception ex)
        {
            Response.StatusCode = 500;
            message = ex.Message;
            //Response.Write(ex);
            //Response.End();
        }

        //Response.Write(jSONString);
        //Response.Flush();
        //Response.End();
    }

    protected override void Render(HtmlTextWriter writer)
    {
        Response.Clear();
        if (flag)
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
