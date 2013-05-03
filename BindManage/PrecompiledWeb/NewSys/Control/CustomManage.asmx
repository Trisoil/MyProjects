<%@ WebService Language="C#" Class="CustomManage" %>

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
/// CustomManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class CustomManage : System.Web.Services.WebService
{

    static string _eventFrom = "������Ϣ����";
    HistoryRecordManage _hrm;
    public CustomManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 

        _hrm = new HistoryRecordManage();
    }

    [WebMethod]
    public CustomJson GetAllCustom(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        CustomJson json = new CustomJson();
        json.CurrentPage=CustomInfo.Execute(new SqlQuery<CustomInfo>("*",start,limit,sort,dir,CustomInfo.ViewName,CustomInfo.PrimaryKey,filters.ToArray(),null)) as CustomInfo[];
        json.TotalCount = (int)CustomInfo.Execute(new SqlCount<CustomInfo>(start, limit, sort, dir, CustomInfo.ViewName, CustomInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddCustom(string cname, string ename, string des, int proId)
    {
        string enName = ename.Trim();
        judgeCustomName(proId, enName);
        CustomInfo info = new CustomInfo();
        info.CnName = cname;
        info.EnName = enName;
        info.Description = des;
        info.ProductID = proId;
        info.Save();
        string eventLog = string.Format("������һ��������Ϣ,����������Ϊ{0},Ӣ����Ϊ{1}", cname, enName);
        _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditCustom(int id, string cname, string ename, string des, int proId)
    {
        string enName = ename.Trim();
        CustomInfo info = CustomInfo.FindById(id);
        if (info != null)
        {
            if (info.ProductID != proId || info.EnName != enName)
                judgeCustomName(proId, enName);
            info.CnName = cname;
            info.EnName = enName;
            info.Description = des;
            info.ProductID = proId;
            info.Save();
            string eventLog = string.Format("�༭��һ��������Ϣ,����������Ϊ{0},Ӣ����Ϊ{1}", cname, enName);
            _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }
    
    [WebMethod]
    public void DeleteCustoms(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            CustomInfo info = CustomInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.CnName);
                builder.Append("/");
                builder2.Append(info.EnName);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}��������Ϣ,����������Ϊ{1},����Ӣ����Ϊ{2}", ids.Length, builder.ToString(),builder2.ToString());
        _hrm.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void judgeCustomName(int productId, string ename)
    {
        string sql = string.Format("select count(*) from {0} where ProductID={1} and EnName='{2}'",CustomInfo.TableName,productId,ename);
        int count = (int)CustomInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("�ò�Ʒ�Ѿ�������ͬӢ�����Ķ�����Ϣ�����޸ĺ������ӣ�");
    }

    [WebMethod]
    public CustomProductJson GetAllProducts()
    {
        CustomProductJson json = new CustomProductJson();
        json.CurrentPage = ProductInfo.FindAll();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [Serializable]
    public class CustomJson
    {
        public int TotalCount;
        public CustomInfo[] CurrentPage;
    }
    
    [Serializable]
    public class CustomProductJson
    {
        public int TotalCount;
        public ProductInfo[] CurrentPage;
    }

}

