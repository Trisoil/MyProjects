<%@ WebService Language="C#" Class="TemplateManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using System.Text;

using Synacast.PPInstall;
using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.New;
using NewWorkFlowSys.HistoryRecord;

/// <summary>
/// TemplateManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class TemplateManage : System.Web.Services.WebService
{
    static string _eventFrom = "捆绑页模版信息";
    HistoryRecordManage _hr;
    public TemplateManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public TemplateJson GetAllTemplate(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        TemplateJson json = new TemplateJson();
        json.CurrentPage = TemplateInfo.Execute(new SqlQuery<TemplateInfo>("*", start, limit, sort, dir, TemplateInfo.TableName, TemplateInfo.PrimaryKey, filters.ToArray(), null)) as TemplateInfo[];
        json.TotalCount = (int)TemplateInfo.Execute(new SqlCount<TemplateInfo>(start, limit, sort, dir, TemplateInfo.TableName, TemplateInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddTemplate(string name,int type, bool enableAll, int softCount, string des)
    {
        JudgeTemplate(name, type);
        TemplateInfo info = new TemplateInfo();
        info.Name = name;
        info.Type = type;
        info.EnableAll = enableAll;
        info.SoftCount = softCount;
        info.Description = des;
        info.Save();
        string eventLog = string.Format("增加了一个捆绑页模版信息,模版名为{0}",name);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditTemplate(int id,string name, int type, bool enableAll, int softCount, string des)
    {
        TemplateInfo info = TemplateInfo.FindById(id);
        if (info != null)
        {
            if (info.Name != name || info.Type != type)
                JudgeTemplate(id, name, type);
            info.Name = name;
            info.Type = type;
            info.EnableAll = enableAll;
            info.SoftCount = softCount;
            info.Description = des;
            info.Save();
            string eventLog = string.Format("编辑了一个捆绑页模版信息,模版名为{0}", name);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteTemplates(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        foreach (int id in ids)
        {
            TemplateInfo info = TemplateInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.Name);
                builder.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个捆绑页模版信息,模版名为{1}", ids.Length,builder.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void JudgeTemplate(string name, int type)
    {
        string sql = string.Format("select count(*) from {0} where Name='{1}' or Type={2}",TemplateInfo.TableName,name,type);
        int count = (int)TemplateInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("已经存在相同名称或类型的模版信息，请修改后继续添加！");
    }

    private void JudgeTemplate(int id, string name, int type)
    {
        string sql = string.Format("select TemplateID from {0} where Name='{1}' or Type={2}", TemplateInfo.TableName, name, type);
        int tempId = (int)TemplateInfo.ExecuteScalarBySql(sql);
        if (tempId != id)
            throw new ApplicationException("已经存在相同名称或类型的模版信息，请修改后继续添加！");
    }

    [Serializable]
    public class TemplateJson
    {
        public int TotalCount;
        public TemplateInfo[] CurrentPage;
    }

}


