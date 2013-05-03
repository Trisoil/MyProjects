using System;
using System.Collections.Generic;
using System.Text;

/// <summary>
/// 典型客户端创建样例, 需在项目增加引用 PPLiveRemoteLibrary.dll
/// </summary>
public class RemoteClient
{
    /// <summary>
    /// 配置信息
    /// </summary>
    private static PPLiveRemoteLibrary.ConfigurationManager myConfigurationManager;

    private static PPLiveRemoteLibrary.PPLiveRemoteManager manager;

    /// <summary>
    /// 创建实例
    /// </summary>
    private RemoteClient(string applicationName)
    {
        this.applicationName = applicationName;
        this.Init();
    }

    /// <summary>
    /// 初始化
    /// </summary>
    private void Init()
    {
        //获取图片管理对象
        this.myImageManager = manager.GetImageManage(myConfigurationManager, this.applicationName);
        //获取视频管理对象
        this.myVideoManager = manager.GetVideoManager(myConfigurationManager, this.applicationName);
    }

    public static RemoteClient GetInstance(string applicationName)
    {
        //读取本地信息
        myConfigurationManager = new PPLiveRemoteLibrary.ConfigurationManager();

        //读取并应用配置           
        PPLiveRemoteLibrary.ConfigurationManager.Configure(System.Web.Configuration.WebConfigurationManager.OpenWebConfiguration("/").FilePath);

        //获取远程对象
        manager = System.Activator.CreateInstance(typeof(PPLiveRemoteLibrary.PPLiveRemoteManager)) as PPLiveRemoteLibrary.PPLiveRemoteManager;

        RemoteClient rc = new RemoteClient(applicationName);

        return rc;
    }

    private readonly string applicationName;
    public string ApplicationName
    {
        get
        {
            return applicationName;
        }
    }

    private PPLiveRemoteLibrary.ImageManager myImageManager;
    /// <summary>
    /// 图片管理
    /// </summary>
    public PPLiveRemoteLibrary.ImageManager Image
    {
        get
        {
            return myImageManager;
        }
    }

    private PPLiveRemoteLibrary.VideoManager myVideoManager;
    /// <summary>
    /// 视频管理
    /// </summary>
    public PPLiveRemoteLibrary.VideoManager Video
    {
        get
        {
            return myVideoManager;
        }
    }

    public override int GetHashCode()
    {
        return this.applicationName.GetHashCode();
    }
}