<%@ WebService Language="C#" Class="VersionManage" %>

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
/// VersionManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class VersionManage : System.Web.Services.WebService
{

    static string _eventFrom = "�汾���ƹ���";
    HistoryRecordManage _hr;
    public VersionManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 

        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public VersionJson GetAllVersion(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        VersionJson json = new VersionJson();
        json.CurrentPage = VersionInfo.Execute(new SqlQuery<VersionInfo>("*", start, limit, sort, dir, VersionInfo.ViewName, VersionInfo.PrimaryKey, filters.ToArray(), null)) as VersionInfo[];
        json.TotalCount = (int)VersionInfo.Execute(new SqlCount<VersionInfo>(start, limit, sort, dir, VersionInfo.ViewName, VersionInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public VersionComboJson GetAllCustoms()
    {
        string sql = string.Format("select * from {0}",CustomInfo.ViewName);
        CustomInfo[] result = CustomInfo.Execute(new SqlCommonQuery<CustomInfo>(sql)) as CustomInfo[];
        VersionComboJson json = new VersionComboJson();
        List<VersionCombo> list = new List<VersionCombo>();
        foreach (CustomInfo info in result)
        {
            VersionCombo combo = new VersionCombo();
            combo.ID = info.CustomID;
            combo.Name = string.Format("{0}|{1}",info.ProductEnName,info.EnName);
            list.Add(combo);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public void AddVersion(string endVersion, string downloadUrl, int customId)
    {
        JudgeCustomId(customId);
        VersionInfo info = new VersionInfo();
        info.EndVersion = endVersion;
        info.DownloadUrl = downloadUrl;
        info.CustomID = customId;
        info.Save();
        string eventLog = string.Format("������һ���汾������Ϣ,�����Ͱ汾��Ϊ{0},��ʾ���ص�ַΪ{1}",endVersion,downloadUrl);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditVersion(int id, string endVersion, string downloadUrl, int customId)
    {
        VersionInfo info = VersionInfo.FindById(id);
        if (info != null)
        {
            if (info.CustomID != customId)
                JudgeCustomId(customId);
            info.EndVersion = endVersion;
            info.DownloadUrl = downloadUrl;
            info.CustomID = customId;
            info.Save();
            string eventLog = string.Format("�༭��һ���汾������Ϣ,�����Ͱ汾��Ϊ{0},��ʾ���ص�ַΪ{1}", endVersion, downloadUrl);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteVersions(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            VersionInfo info = VersionInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.EndVersion);
                builder.Append("/");
                builder2.Append(info.DownloadUrl);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}���汾������Ϣ,�����Ͱ汾��Ϊ{1},��ʾ���ص�ַΪ{2}",ids.Length,builder.ToString(),builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void JudgeCustomId(int customId)
    {
        string sql = string.Format("select count(*) from {0} where CustomID={1}",VersionInfo.TableName,customId);
        int count = (int)VersionInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("�Ѿ�������ͬ���Ƶİ汾��Ϣ����ѡ���������Ƽ�����ӣ�");
    }
    
    [Serializable]
    public class VersionJson
    {
        public int TotalCount;
        public VersionInfo[] CurrentPage;
    }

    [Serializable]
    public class VersionComboJson
    {
        public int TotalCount;
        public VersionCombo[] CurrentPage;
    }
    
    [Serializable]
    public class VersionCombo
    {
        public int ID;
        public string Name;
    }

}


