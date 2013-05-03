<%@ WebService Language="C#" Class="BindManage" %>

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
using RmtInterface;
using RmtLibraryAgent;
using NewWorkFlowSys.HistoryRecord;

/// <summary>
/// BindManage 的摘要说明
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class BindManage : System.Web.Services.WebService
{

    static string _eventFrom = "捆绑控制信息";
    HistoryRecordManage _hr;
    public BindManage()
    {

        //如果使用设计的组件，请取消注释以下行 
        //InitializeComponent(); 
        _hr = new HistoryRecordManage();
    }

    [WebMethod]
    public BindJson GetAllBind(int start, int limit, string sort, string dir, List<string[]> filters, List<string[]> inputFilters)
    {
        filters.AddRange(inputFilters);
        BindJson json = new BindJson();
        json.CurrentPage = BindInfo.Execute(new SqlQuery<BindInfo>("*", start, limit, sort, dir, BindInfo.ViewName, BindInfo.PrimaryKey, filters.ToArray(), null)) as BindInfo[];
        json.TotalCount = (int)BindInfo.Execute(new SqlCount<BindInfo>(start, limit, sort, dir, BindInfo.ViewName, BindInfo.PrimaryKey, filters.ToArray(), null));
        return json;
    }

    [WebMethod]
    public void AddBind(string popPage, int customId, int channelId, int isStart, int isWindowsStart,int bindModel)
    {
        JudgeBind(customId, channelId, bindModel);
        BindInfo info = new BindInfo();
        info.PopupPage = popPage;
        info.CustomID = customId;
        info.ChannelID = channelId;
        info.IsStart = isStart;
        info.IsStartWithWindows = isWindowsStart;
        info.BindMode = bindModel;
        info.Save();
        string eventLog = string.Format("增加了一个捆绑控制信息,定制标识号为{0},渠道标识号为{1}", customId, channelId);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    [WebMethod]
    public void EditBind(int id, string popPage, int customId, int channelId, int isStart, int isWindowsStart,int bindModel)
    {
        BindInfo info = BindInfo.FindById(id);
        if (info != null)
        {
            if (info.CustomID != customId || info.ChannelID != channelId || info.BindMode != bindModel)
                JudgeBind(customId, channelId, bindModel);
            info.PopupPage = popPage;
            info.CustomID = customId;
            info.ChannelID = channelId;
            info.IsStart = isStart;
            info.IsStartWithWindows = isWindowsStart;
            info.BindMode = bindModel;
            info.Save();
            string eventLog = string.Format("编辑了一个捆绑控制信息,定制标识号为{0},渠道标识号为{1}", customId, channelId);
            _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
        }
    }

    [WebMethod]
    public void DeleteBinds(int[] ids)
    {
        StringBuilder builder = new StringBuilder();
        StringBuilder builder2 = new StringBuilder();
        foreach (int id in ids)
        {
            BindInfo info = BindInfo.FindById(id);
            if (info != null)
            {
                builder.Append(info.CustomID);
                builder.Append("/");
                builder2.Append(info.ChannelID);
                builder2.Append("/");
                info.Delete();
            }
        }
        string eventLog = string.Format("删除了{0}个捆绑控制信息,定制标识号为{1},渠道标识号为{2}", ids.Length, builder.ToString(), builder2.ToString());
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, eventLog);
    }

    private void JudgeBind(int customId, int channelId, int bindModel)
    {
        string sql = string.Format("select count(*) from {0} where CustomID={1} and ChannelID={2} and BindMode={3}", BindInfo.TableName, customId, channelId, bindModel);
        int count = (int)BindInfo.ExecuteScalarBySql(sql);
        if (count > 0)
            throw new ApplicationException("已经存在相同的捆绑控制信息，请修改后继续添加！");
    }

    [WebMethod]
    public void MakeChannel(string customId, string channelId)
    {
        string errMsg;
        IRemotingFile ro = RemotingManager.GetManager().GetObject<IRemotingFile>(out errMsg);
        bool flag = ro.ProduceChannelSetupFiles(customId, channelId, out errMsg);
        _hr.SaveEventLog(PPInstallHelp.CurrentUserName, PPInstallHelp.ClientIp, _eventFrom, errMsg);
        if (!flag)
        {
            throw new ApplicationException(errMsg);
        }
    }
    
    [WebMethod]
    public bindCustomJson GetAllCustoms()
    {
        string sql = string.Format("select * from {0}",CustomInfo.ViewName);
        CustomInfo[] result = CustomInfo.Execute(new SqlCommonQuery<CustomInfo>(sql)) as CustomInfo[];
        bindCustomJson json = new bindCustomJson();
        List<bindCustom> list = new List<bindCustom>();
        foreach (CustomInfo info in result)
        {
            bindCustom combo = new bindCustom();
            combo.ID = info.CustomID;
            combo.Name = string.Format("{0}|{1}",info.ProductEnName,info.EnName);
            list.Add(combo);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }

    [WebMethod]
    public BindChannelJson GetAllChannel()
    {
        ChannelInfo[] result = ChannelInfo.FindAll();
        BindChannelJson json = new BindChannelJson();
        List<BindChannel> list = new List<BindChannel>();
        foreach (ChannelInfo info in result)
        {
            BindChannel channel = new BindChannel();
            channel.ID = info.ChannelID;
            channel.Name = info.EnName;   
            list.Add(channel);
        }
        json.CurrentPage = list.ToArray();
        json.TotalCount = json.CurrentPage.Length;
        return json;
    }
    
    [Serializable]
    public class BindJson
    {
        public int TotalCount;
        public BindInfo[] CurrentPage;
    }

    [Serializable]
    public class bindCustomJson
    {
        public int TotalCount;
        public bindCustom[] CurrentPage;
    }

    [Serializable]
    public class bindCustom
    {
        public int ID;
        public string Name;
    }

    [Serializable]
    public class BindChannelJson
    {
        public int TotalCount;
        public BindChannel[] CurrentPage;
    }

    [Serializable]
    public class BindChannel
    {
        public int ID;
        public string Name;
    }
    
}


