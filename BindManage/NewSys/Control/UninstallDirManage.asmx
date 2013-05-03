<%@ WebService Language="C#" Class="UninstallDirManage" %>

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
/// UninstallDirManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class UninstallDirManage : System.Web.Services.WebService
{
    static string _eventFrom = "ж�����Ŀ¼";
    HistoryRecordManage _hr;
    public UninstallDirManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public UninstallDirJson GetUninstallDirById(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters, int softId)
    {
        filters.AddRange(inputFilters);
        string where = string.Format(" and SoftwareID={0}", softId);
        UninstallDirJson json = new UninstallDirJson();
        json.CurrentPage = UninstallDirInfo.Execute(new SqlQuery<UninstallDirInfo>("*", start, limit, sort, dir, UninstallDirInfo.TableName, UninstallDirInfo.PrimaryKey, filters.ToArray(), where)) as UninstallDirInfo[];
        json.TotalCount = (int)UninstallDirInfo.Execute(new SqlCount<UninstallDirInfo>(start, limit, sort, dir, UninstallDirInfo.TableName, UninstallDirInfo.PrimaryKey, filters.ToArray(), where));
        return json;
    }

    [WebMethod]
    public void AddUninstallDir(int softId, string unDirDir, string unDirFolder,string unDirValue)
    {
        UninstallDirInfo info = new UninstallDirInfo();
        info.SoftwareID = softId;
        info.DelDir = unDirDir;
        info.DelFolder = unDirFolder;
        info.DelValue = unDirValue;
        info.Save();
        string eventLog = string.Format("������һ��ж�����Ŀ¼��Ϣ,���Ŀ��Ŀ¼{0},���Ŀ���ļ���Ϊ{1}", unDirDir,unDirFolder);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditUninstallDir(int id, int softId, string unDirDir, string unDirFolder,string unDirValue)
    {
        UninstallDirInfo info = UninstallDirInfo.FindById(id);
        if (info != null)
        {
            info.SoftwareID = softId;
            info.DelDir = unDirDir;
            info.DelFolder = unDirFolder;
            info.DelValue = unDirValue;
            info.Save();
            string eventLog = string.Format("�༭��һ��ж�����Ŀ¼��Ϣ,���Ŀ��Ŀ¼{0},���Ŀ���ļ���Ϊ{1}", unDirDir, unDirFolder);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteUninstallDirs(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            UninstallDirInfo info = UninstallDirInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.DelDir);
                builder.Append("/");
                builder2.Append(info.DelFolder);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}��ж�����Ŀ¼��Ϣ,���Ŀ��Ŀ¼{1},���Ŀ���ļ���Ϊ{2}", ids.Length, builder.ToString(), builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [Serializable]
    public class UninstallDirJson
    {
        public int TotalCount;
        public UninstallDirInfo[] CurrentPage;
    }
    
}


