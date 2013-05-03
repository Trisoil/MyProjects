<%@ WebService Language="C#" Class="ProductManage" %>

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
/// ProductManage ��ժҪ˵��
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class ProductManage : System.Web.Services.WebService
{
    static string _eventFrom = "���²�Ʒ����";
    HistoryRecordManage _hr;
    public ProductManage()
    {

        //���ʹ����Ƶ��������ȡ��ע�������� 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }


    [WebMethod]
    public ProductJson GetAllProduct(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        ProductJson json = new ProductJson();
        json.CurrentPage = ProductInfo.Execute(new SqlQuery<ProductInfo>("*", start, limit, sort, dir, ProductInfo.TableName, ProductInfo.PrimaryKey, filters.ToArray(), null)) as ProductInfo[];
        json.TotalCount = (int)ProductInfo.Execute(new SqlCount<ProductInfo>(start, limit, sort, dir, ProductInfo.TableName, ProductInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddProduct(string cname, string ename, string cdes, string edes)
    {
        string enName = ename.Trim();
        JudgeProductName(enName);
        ProductInfo pro = new ProductInfo();
        pro.CnName = cname;
        pro.EnName = enName;
        pro.CnDescription = cdes;
        pro.EnDescription = edes;
        pro.Save();
        string eventLog = string.Format("������һ���µĲ�Ʒ,������Ϊ{0},Ӣ����Ϊ{1}", cname, enName);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName,PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditProduct(int id, string cname, string ename, string cdes, string edes)
    {
        string enName = ename.Trim();
        ProductInfo pro = ProductInfo.FindById(id);
        if (pro != null)
        {
            if (pro.EnName != enName)
                JudgeProductName(enName);
            pro.CnName = cname;
            pro.EnName = enName;
            pro.CnDescription = cdes;
            pro.EnDescription = edes;
            pro.Save();
            string eventLog = string.Format("�༭��һ����Ʒ,������Ϊ{0},Ӣ����Ϊ{1}", cname, enName);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteProducts(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            ProductInfo info = ProductInfo.FindById(id);
            if (info != null)
            {
                builder2.Append(info.CnName);
                builder2.Append("/");
                builder.Append(info.EnName);
                builder.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("ɾ����{0}����Ʒ,������Ϊ{1},Ӣ����Ϊ{2}",ids.Length,builder2.ToString(),builder.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void JudgeProductName(string eName)
    {
        string sql = string.Format("select count(*) from {0} where EnName='{1}'",ProductInfo.TableName,eName);
        int count = (int)ProductInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("�Ѿ�������ͬӢ�����Ĳ�Ʒ�����޸ĺ������ӣ�");
    }
    
    [Serializable]
    public class ProductJson
    {
        public int TotalCount;
        public ProductInfo[] CurrentPage;
    }

}


