<%@ WebService Language="C#" Class="SoftManage" %>

using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using System.Text;

using Synacast.PPInstall;
using Synacast.PPInstall.Repository;
using Synacast.NHibernateActiveRecord;
using Synacast.NHibernateActiveRecord.New;
using NewWorkFlowSys.HistoryRecord;

/// <summary>
/// SoftManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class SoftManage : System.Web.Services.WebService
{
    static string _eventFrom = "捆绑软件管理";
    HistoryRecordManage _hr;
    IPAreaRepository _ipRepository;
    public SoftManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
        _ipRepository = new IPAreaRepository();
    }

    [WebMethod]
    public SoftJson GetAllSoft(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        SoftJson json = new SoftJson();
        json.CurrentPage = SoftInfo.Execute(new SqlQuery<SoftInfo>("*", start, limit, sort, dir, SoftInfo.TableName, SoftInfo.PrimaryKey, filters.ToArray(), null)) as SoftInfo[];
        json.TotalCount = (int)SoftInfo.Execute(new SqlCount<SoftInfo>(start, limit, sort, dir, SoftInfo.TableName, SoftInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddSoft(int type, string cName, string eName, string cDes, string eDes, string url, string imageUrl, string par, string mainPage, string fPage, string exPage, string iconDownUrl, string iconLinkUrl, string iconName, string desktopName, string desktopEnName, bool isChecked, bool bindEnable, int countPerDay)
    {
        SoftInfo info = new SoftInfo();
        info.Type = type;
        info.CnName = cName;
        info.EnName = eName;
        info.CnDescription = cDes;
        info.EnDescription = eDes;
        info.Url = url;
        info.ImageUrl = imageUrl;
        info.Parameter = par;
        info.MainPage = mainPage;
        info.ForcePage = fPage;
        info.ExcludePage = exPage;
        info.IconDownloadUrl = iconDownUrl;
        info.IconLinkUrl = iconLinkUrl;
        info.IconName = iconName;
        info.DesktopName = desktopName;
        info.DesktopEnName = desktopEnName;
        info.IsChecked = isChecked;
        info.BindEnable = bindEnable;
        info.CountPerDay = countPerDay;
        info.Save();
        string eventLog = string.Format("增加了一个捆绑软件,软件中文名为{0},英文名为{1}", cName, eName);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditSoft(int id, int type, string cName, string eName, string cDes, string eDes, string url, string imageUrl, string par, string mainPage, string fPage, string exPage, string iconDownUrl, string iconLinkUrl, string iconName, string desktopName, string desktopEnName, bool isChecked, bool bindEnable, int countPerDay)
    {
        SoftInfo info = SoftInfo.FindById(id);
        if (info != null)
        {
            info.Type = type;
            info.CnName = cName;
            info.EnName = eName;
            info.CnDescription = cDes;
            info.EnDescription = eDes;
            info.Url = url;
            info.ImageUrl = imageUrl;
            info.Parameter = par;
            info.MainPage = mainPage;
            info.ForcePage = fPage;
            info.ExcludePage = exPage;
            info.IconDownloadUrl = iconDownUrl;
            info.IconLinkUrl = iconLinkUrl;
            info.IconName = iconName;
            info.DesktopName = desktopName;
            info.DesktopEnName = desktopEnName;
            info.IsChecked = isChecked;
            info.BindEnable = bindEnable;
            info.CountPerDay = countPerDay;
            info.Save();
            string eventLog = string.Format("编辑了一个捆绑软件,软件中文名为{0},英文名为{1}", cName, eName);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteSofts(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            SoftInfo info = SoftInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.CnName);
                builder.Append("/");
                builder2.Append(info.EnName);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个捆绑软件,软件中文名为{1},英文名为{2}", ids.Length, builder.ToString(), builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void BindSoftEnable(int[] keys, bool enable)
    {
        StringBuilder builder = new StringBuilder();
        foreach (int key in keys)
        {
            SoftInfo info = SoftInfo.FindById(key);
            if (info != null)
            {
                builder.Append(info.CnName);
                builder.Append("/");
                info.BindEnable = enable;
                info.Save();
            }
        }
        string state = "无效";
        if (enable)
            state = "有效";
        string eventLog = string.Format("将{0}个捆绑软件{1}的绑定状态置为{2}", keys.Length, builder.ToString(), state);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public CityCodeInfo[] GetAllCity()
    {
        return CityCodeInfo.FindAll();
    }

    [WebMethod]
    public IPAreaNode[] LoadIPAreaTree()
    {
        return _ipRepository.GetIPAreaTree();
    }

    [WebMethod]
    public void SaveIPAreas(int id, string areas, string worb)
    {
        SoftInfo info = SoftInfo.FindById(id);
        if (info != null)
        {
            if (worb == "soft_white")
                info.WhiteCityCodes = areas;
            else
                info.BlackCityCodes = areas;
            info.Save();
        }
    }

    [WebMethod]
    public void SaveAreaType(int id, int type)
    {
        SoftInfo info = SoftInfo.FindById(id);
        if (info != null)
        {
            info.AreaControlType = type;
            info.Save();
        }
    }

    [WebMethod]
    public IPAreaGroupInfo[] GetAllIPS()
    {
        return IPAreaGroupInfo.FindAll();
    }

    [WebMethod]
    public string SaveIPAreaGroup(int id, string areas, string[] offareas, string worb)
    {
        SoftInfo info = SoftInfo.FindById(id);
        if (info != null)
        {
            string truearea = string.Empty;
            if (worb == "soft_white")
            {
                truearea = FormatIPAreaGroup(info.WhiteCityCodes, areas, offareas);
                info.WhiteCityCodes = truearea;
            }
            else
            {
                truearea = FormatIPAreaGroup(info.BlackCityCodes, areas, offareas);
                info.BlackCityCodes = truearea;
            }
            info.Save();
            return truearea;
        }
        return string.Empty;
    }

    private string FormatIPAreaGroup(string resource, string areas, string[] offareas)
    {
        string[] res = resource.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
        foreach (string re in res)
        {
            if (areas.Contains(re))
                resource = resource.Replace(re, "");
        }
        string trueres = string.Format("{0} {1}", resource, areas);
        foreach (string offa in offareas)
        {
            if (trueres.Contains(offa))
                trueres = trueres.Replace(offa, "");
        }
        return string.Join(" ", trueres.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries));
    }

    [Serializable]
    public class SoftJson
    {
        public int TotalCount;
        public SoftInfo[] CurrentPage;
    }
}
