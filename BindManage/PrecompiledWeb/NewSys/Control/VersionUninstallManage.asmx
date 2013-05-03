<%@ WebService Language="C#" Class="VersionUninstallManage" %>

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
/// VersionUninstallManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class VersionUninstallManage : System.Web.Services.WebService
{

    static string _eventFrom = "附加冲突软件";
    HistoryRecordManage _hr;
    public VersionUninstallManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public VersionUninstallJson GetVersionUninstallById(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters, int versionId)
    {
        filters.AddRange(inputFilters);
        string where = string.Format(" and VersionID={0}", versionId);
        VersionUninstallJson json = new VersionUninstallJson();
        json.CurrentPage = VersionUninstallInfo.Execute(new SqlQuery<VersionUninstallInfo>("*", start, limit, sort, dir, VersionUninstallInfo.ViewName, VersionUninstallInfo.PrimaryKey, filters.ToArray(), where)) as VersionUninstallInfo[];
        json.TotalCount = (int)VersionUninstallInfo.Execute(new SqlCount<VersionUninstallInfo>(start, limit, sort, dir, VersionUninstallInfo.ViewName, VersionUninstallInfo.PrimaryKey, filters.ToArray(), where));
        return json;
    }

    [WebMethod]
    public void AddVersionUninstall(int versionId, int softId)
    {
        JudgeVersionUninstall(versionId, softId);
        VersionUninstallInfo info = new VersionUninstallInfo();
        info.SoftwareID = softId;
        info.VersionID = versionId;
        info.Save();
        UpdateVersionAdd(versionId);
        string eventLog = string.Format("附加了一个冲突软件,版本控制标识号为{0},卸载软件标识号为{1}", versionId, softId);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditVersionUninstall(int id, int versionId, int softId)
    {
        VersionUninstallInfo info = VersionUninstallInfo.FindById(id);
        if (info != null)
        {
            if (info.SoftwareID != softId)
                JudgeVersionUninstall(versionId, softId);
            info.SoftwareID = softId;
            info.VersionID = versionId;
            info.Save();
            string eventLog = string.Format("编辑了一个冲突软件,版本控制标识号为{0},卸载软件标识号为{1}", versionId, softId);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteVersionUninstalls(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            VersionUninstallInfo info = VersionUninstallInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.VersionID);
                builder.Append("/");
                builder2.Append(info.SoftwareID);
                builder2.Append("/");
                int vid = info.VersionID;
                info.Delete();
                UpdateVersionDes(vid);
            }
        }
        string eventLog = string.Format("删除了{0}个冲突软件,版本控制标识号为{1},卸载软件标识号为{2}",ids.Length,builder.ToString(),builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    /// <summary>
    /// 下拉框得到所有的卸载软件
    /// </summary>
    /// <returns></returns>
    [WebMethod]
    public VersionUninstallComboJson GetAllUninstallSoft()
    {
        VersionUninstallComboJson json = new VersionUninstallComboJson();
        json.CurrentPage = UninstallSoftInfo.FindAll();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    /// <summary>
    /// 判断是否已经添加相同的冲突软件
    /// </summary>
    /// <param name="versionId"></param>
    /// <param name="softId"></param>
    private void JudgeVersionUninstall(int versionId, int softId)
    {
        string sql = string.Format("select count(*) from {0} where VersionID={1} and SoftwareID={2}",VersionUninstallInfo.TableName,versionId,softId);
        int count = (int)VersionUninstallInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("已经存在相同的冲突软件，请选择其他软件！");
    }
    
    /// <summary>
    /// 更新版本控制信息中的冲突软件个数
    /// </summary>
    /// <param name="versionId"></param>
    private void UpdateVersionAdd(int versionId)
    {
        VersionInfo version = VersionInfo.FindById(versionId);
        if (version != null)
        {
            version.SoftwareCount += 1;
            version.Save();
        }
    }

    /// <summary>
    /// 更新版本控制信息中的冲突软件个数
    /// </summary>
    /// <param name="versionId"></param>
    private void UpdateVersionDes(int versionId)
    {
        VersionInfo version = VersionInfo.FindById(versionId);
        if (version != null)
        {
            version.SoftwareCount -= 1;
            version.Save();
        }
    }

    [Serializable]
    public class VersionUninstallJson
    {
        public int TotalCount;
        public VersionUninstallInfo[] CurrentPage;
    }

    [Serializable]
    public class VersionUninstallComboJson
    {
        public int TotalCount;
        public UninstallSoftInfo[] CurrentPage;
    }
}


