<%@ WebService Language="C#" Class="UninstallRegManage" %>

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
/// UninstallRegManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class UninstallRegManage : System.Web.Services.WebService
{

    static string _eventFrom = "卸载软件目录";
    HistoryRecordManage _hr;
    
    public UninstallRegManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public UninstallRegJson GetUninstallRegById(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters, int softId)
    {
        filters.AddRange(inputFilters);
        string where = string.Format(" and SoftwareID={0}", softId);
        UninstallRegJson json = new UninstallRegJson();
        json.CurrentPage = UninstallRegInfo.Execute(new SqlQuery<UninstallRegInfo>("*", start, limit, sort, dir, UninstallRegInfo.TableName, UninstallRegInfo.PrimaryKey, filters.ToArray(), where)) as UninstallRegInfo[];
        json.TotalCount = (int)UninstallRegInfo.Execute(new SqlCount<UninstallRegInfo>(start, limit, sort, dir, UninstallRegInfo.TableName, UninstallRegInfo.PrimaryKey, filters.ToArray(), where));
        return json;
    }

    [WebMethod]
    public void AddUninstallReg(int softId, string rootReg, string subReg,string delValue)
    {
        UninstallRegInfo info = new UninstallRegInfo();
        info.SoftwareID = softId;
        info.DelRootReg = rootReg;
        info.DelSubReg = subReg;
        info.DelValue = delValue;
        info.Save();
        string eventLog = string.Format("增加了一个卸载软件注册表信息,注册表根名{0},注册表子路径{1}", rootReg, subReg);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditUninstallReg(int id, int softId, string rootReg, string subReg,string delValue)
    {
        UninstallRegInfo info = UninstallRegInfo.FindById(id);
        if (info != null)
        {
            info.SoftwareID = softId;
            info.DelRootReg = rootReg;
            info.DelSubReg = subReg;
            info.DelValue = delValue;
            info.Save();
            string eventLog = string.Format("编辑了一个卸载软件注册表信息,注册表根名{0},注册表子路径{1}", rootReg, subReg);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteUninstallRegs(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            UninstallRegInfo info = UninstallRegInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.DelRootReg);
                builder.Append("/");
                builder2.Append(info.DelSubReg);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个卸载软件注册表信息,注册表根名{1},注册表子路径{2}", ids.Length, builder.ToString(), builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }
    
    [Serializable]
    public class UninstallRegJson
    {
        public int TotalCount;
        public UninstallRegInfo[] CurrentPage;
    }
}


