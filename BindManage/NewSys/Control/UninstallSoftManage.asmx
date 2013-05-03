<%@ WebService Language="C#" Class="UninstallSoftManage" %>

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
/// UninstallSoftManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class UninstallSoftManage : System.Web.Services.WebService
{

    static string _eventFrom = "ж���������";
    HistoryRecordManage _hr;
    public UninstallSoftManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public UninstallSoftJson GetAllUninstallSoft(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        UninstallSoftJson json = new UninstallSoftJson();
        json.CurrentPage = UninstallSoftInfo.Execute(new SqlQuery<UninstallSoftInfo>("*", start, limit, sort, dir, UninstallSoftInfo.TableName, UninstallSoftInfo.PrimaryKey, filters.ToArray(), null)) as UninstallSoftInfo[];
        json.TotalCount = (int)UninstallSoftInfo.Execute(new SqlCount<UninstallSoftInfo>(start, limit, sort, dir, UninstallSoftInfo.TableName, UninstallSoftInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddUninstallSoft(string name, string unDirName, string unProcess, string unDir, int unForce, int unMust)
    {
        UninstallSoftInfo info = new UninstallSoftInfo();
        info.Name = name;
        info.UninstallName = unDirName;
        info.KillExe = unProcess;
        info.AppPath = unDir;
        info.ForceUninstall = unForce;
        info.IsMust = unMust;
        info.Save();
        string eventLog = string.Format("������һ��ж�������Ϣ,�����Ϊ{0}",name);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditUninstallSoft(int id, string name, string unDirName, string unProcess, string unDir, int unForce, int unMust)
    {
        UninstallSoftInfo info = UninstallSoftInfo.FindById(id);
        if (info != null)
        {
            info.Name = name;
            info.UninstallName = unDirName;
            info.KillExe = unProcess;
            info.AppPath = unDir;
            info.ForceUninstall = unForce;
            info.IsMust = unMust;
            info.Save();
            string eventLog = string.Format("�༭��һ��ж�������Ϣ,�����Ϊ{0}", name);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteUninstallSofts(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        foreach (int id in ids)
        {
            UninstallSoftInfo info = UninstallSoftInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.Name);
                builder.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}��ж�������Ϣ,�����Ϊ{1}", ids.Length,builder.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [Serializable]
    public class UninstallSoftJson
    {
        public int TotalCount;
        public UninstallSoftInfo[] CurrentPage;
    }

}


