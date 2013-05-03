<%@ WebService Language="C#" Class="ChannelManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using System.Text;

using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.New;
using Synacast.PPInstall;
using NewWorkFlowSys.HistoryRecord;

/// <summary>
/// ChannelManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class ChannelManage : System.Web.Services.WebService
{

    static string _eventFrom = "推广渠道管理";
    HistoryRecordManage _hrm;
    
    public ChannelManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hrm = new HistoryRecordManage();
    }

    [WebMethod]
    public ChannelJson GetAllChannel(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        ChannelJson json = new ChannelJson();
        json.CurrentPage = ChannelInfo.Execute(new SqlQuery<ChannelInfo>("*", start, limit, sort, dir, ChannelInfo.TableName, ChannelInfo.PrimaryKey, filters.ToArray(), null)) as ChannelInfo[];
        json.TotalCount = (int)ChannelInfo.Execute(new SqlCount<ChannelInfo>(start, limit, sort, dir, ChannelInfo.TableName, ChannelInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddChannel(string cName, string eName, string desc, string type)
    {
        string enName = eName.Trim();
        JudgeChannel(enName);
        ChannelInfo info = new ChannelInfo();
        info.CnName = cName;
        info.EnName = enName;
        info.Description = desc;
        info.Type = type;
        info.Save();
        string eventLog = string.Format("增加了一个推广渠道信息,渠道中文名为{0},英文名为{1}", cName, enName);
        _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditChannel(int id, string cName, string eName, string desc, string type)
    {
        string enName = eName.Trim();
        ChannelInfo info = ChannelInfo.FindById(id);
        if (info != null)
        {
            if (info.EnName != enName)
                JudgeChannel(enName);
            info.CnName = cName;
            info.EnName = enName;
            info.Description = desc;
            info.Type = type;
            info.Save();
            string eventLog = string.Format("编辑了一个推广渠道信息,渠道中文名为{0},英文名为{1}", cName, enName);
            _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteChannels(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            ChannelInfo info = ChannelInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.CnName);
                builder.Append("/");
                builder2.Append(info.EnName);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个推广渠道信息,渠道中文名为{1},渠道英文名为{2}", ids.Length, builder.ToString(), builder2.ToString());
        _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void JudgeChannel(string eName)
    {
        string sql = string.Format("select count(*) from {0} where lower(EnName)='{1}'",ChannelInfo.TableName,eName.ToLower());
        int count = (int)ChannelInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("已经存在相同英文名的渠道信息，请修改后继续添加！");
    }

    [Serializable]
    public class ChannelJson
    {
        public int TotalCount;
        public ChannelInfo[] CurrentPage;
    }
}


