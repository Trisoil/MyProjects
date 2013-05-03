using System;
using System.Collections.Generic;
using System.Text;

/// <summary>
/// ���Ϳͻ��˴�������, ������Ŀ�������� PPLiveRemoteLibrary.dll
/// </summary>
public class RemoteClient
{
    /// <summary>
    /// ������Ϣ
    /// </summary>
    private static PPLiveRemoteLibrary.ConfigurationManager myConfigurationManager;

    private static PPLiveRemoteLibrary.PPLiveRemoteManager manager;

    /// <summary>
    /// ����ʵ��
    /// </summary>
    private RemoteClient(string applicationName)
    {
        this.applicationName = applicationName;
        this.Init();
    }

    /// <summary>
    /// ��ʼ��
    /// </summary>
    private void Init()
    {
        //��ȡͼƬ�������
        this.myImageManager = manager.GetImageManage(myConfigurationManager, this.applicationName);
        //��ȡ��Ƶ�������
        this.myVideoManager = manager.GetVideoManager(myConfigurationManager, this.applicationName);
    }

    public static RemoteClient GetInstance(string applicationName)
    {
        //��ȡ������Ϣ
        myConfigurationManager = new PPLiveRemoteLibrary.ConfigurationManager();

        //��ȡ��Ӧ������           
        PPLiveRemoteLibrary.ConfigurationManager.Configure(System.Web.Configuration.WebConfigurationManager.OpenWebConfiguration("/").FilePath);

        //��ȡԶ�̶���
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
    /// ͼƬ����
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
    /// ��Ƶ����
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