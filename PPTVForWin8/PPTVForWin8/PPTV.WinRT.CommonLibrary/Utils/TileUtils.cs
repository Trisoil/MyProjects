using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Notifications;
using Windows.UI.StartScreen;

namespace PPTV.WinRT.CommonLibrary.Utils
{
    using PPTVData.Entity;
    using PPTVData.Entity.Cloud;
    
    public static class TileUtils
    {
        public static void CreateLogoNotifications()
        {
            TileUpdater notifier = TileUpdateManager.CreateTileUpdaterForApplication();
            if (notifier.Setting == NotificationSetting.Enabled)
            {
                notifier.EnableNotificationQueue(true);

                var MainSquareTemplate = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquareImage);
                MainSquareTemplate.GetElementsByTagName("image")[0].Attributes[1].NodeValue = "ms-appx:///Assets/Logo.png";
                var squareNode = MainSquareTemplate.CreateAttribute("branding");
                squareNode.Value = "name";  //none|logo|name
                MainSquareTemplate.GetElementsByTagName("binding")[0].Attributes.SetNamedItem(squareNode);

                var MainWideTemplate = TileUpdateManager.GetTemplateContent(TileTemplateType.TileWideImage);
                MainWideTemplate.GetElementsByTagName("image")[0].Attributes[1].NodeValue = "ms-appx:///Assets/WideLogo.png";
                var wideNode = MainWideTemplate.CreateAttribute("branding");
                wideNode.Value = "none";
                MainWideTemplate.GetElementsByTagName("binding")[0].Attributes.SetNamedItem(wideNode);

                var Mainnode = MainWideTemplate.ImportNode(MainSquareTemplate.GetElementsByTagName("binding").Item(0), true);
                MainWideTemplate.GetElementsByTagName("visual").Item(0).AppendChild(Mainnode);
                var MaintileNotification = new TileNotification(MainWideTemplate);
                notifier.Update(MaintileNotification);
            }
        }

        public static void CreateLocalNotifications(CoverInfo item)
        {
            TileUpdater notifier = TileUpdateManager.CreateTileUpdaterForApplication();
            if (notifier.Setting == NotificationSetting.Enabled)
            {
                notifier.EnableNotificationQueue(true);

                var SquareTemplate = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquarePeekImageAndText04);
                SquareTemplate.GetElementsByTagName("image")[0].Attributes[1].NodeValue = item.ImageUri;//"ms-appdata:///local/" + item.Name;
                SquareTemplate.GetElementsByTagName("text")[0].AppendChild(SquareTemplate.CreateTextNode(item.Title));

                var WideTemplate = TileUpdateManager.GetTemplateContent(TileTemplateType.TileWideImageAndText01);
                WideTemplate.GetElementsByTagName("image")[0].Attributes[1].NodeValue = item.ImageUri; //"ms-appdata:///local/" + item.Name;
                WideTemplate.GetElementsByTagName("text")[0].AppendChild(WideTemplate.CreateTextNode(item.Title));
                //WideTemplate.GetElementsByTagName("text")[1].AppendChild(WideTemplate.CreateTextNode(item.Note));

                var node = WideTemplate.ImportNode(SquareTemplate.GetElementsByTagName("binding").Item(0), true);
                WideTemplate.GetElementsByTagName("visual").Item(0).AppendChild(node);
                var tileNotification = new TileNotification(WideTemplate);
                notifier.Update(tileNotification);
            }
        }

        public static void CreateSecondTileNotifications(string tileId, string imageUri, string text, string subText)
        {
            TileUpdater notifier = TileUpdateManager.CreateTileUpdaterForSecondaryTile(tileId);
            if (notifier.Setting == NotificationSetting.Enabled)
            {
                notifier.EnableNotificationQueue(true);

                var SquareTemplate = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquarePeekImageAndText03);
                SquareTemplate.GetElementsByTagName("image")[0].Attributes[1].NodeValue = imageUri;
                SquareTemplate.GetElementsByTagName("text")[0].AppendChild(SquareTemplate.CreateTextNode(text));
                SquareTemplate.GetElementsByTagName("text")[1].AppendChild(SquareTemplate.CreateTextNode(subText));

                var tileNotification = new TileNotification(SquareTemplate);
                notifier.Update(tileNotification);
            }
        }

        public static void CreateToastNotifications(string content)
        {
            var toastNotifier = ToastNotificationManager.CreateToastNotifier();
            if (toastNotifier.Setting == NotificationSetting.Enabled)
            {
                var template = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);
                var tileAttributes = template.GetElementsByTagName("text")[0];
                tileAttributes.AppendChild(template.CreateTextNode(content));
                var notifications1 = new ToastNotification(template);
                toastNotifier.Show(notifications1);
            }
        }

        public static void CreateToastNotificationsText02(string text01, string text02)
        {
            var toastNotifier = ToastNotificationManager.CreateToastNotifier();
            if (toastNotifier.Setting == NotificationSetting.Enabled)
            {
                var template = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText02);
                var tileAttributes = template.GetElementsByTagName("text")[0];
                tileAttributes.AppendChild(template.CreateTextNode(text01));
                var tileAttributes02 = template.GetElementsByTagName("text")[1];
                tileAttributes02.AppendChild(template.CreateTextNode(text02));
                var notifications1 = new ToastNotification(template);
                toastNotifier.Show(notifications1);
            }
        }

        public async static Task Pin_Start(object dataSource, bool isRecent, Uri tileLog)
        { 
            string tileId = string.Empty;
            string tileName = string.Empty;
            string imageUri = string.Empty;
            string args = string.Empty;

            var cloud = dataSource as CloudDataInfo;
            if (cloud != null)
            {
                tileId = cloud.Id.ToString();
                tileName = cloud.Name;
                imageUri = cloud.ImageUri;
                args = isRecent ? string.Format("{0},{1}", tileId, cloud.ProgramIndex) : tileId;
            }

            var local = dataSource as ChannelDetailInfo;
            if (local != null)
            {
                tileId = local.Id.ToString();
                tileName = local.Title;
                imageUri = local.ImageUri;
                args = tileId;
            }

            if (!string.IsNullOrEmpty(tileId))
            {
                var tile = new SecondaryTile(tileId, tileName, tileName, args, TileOptions.ShowNameOnLogo, tileLog);

                if (await tile.RequestCreateAsync())
                {
                    var subName = isRecent ? "点击继续观看" : "点击查看详情";
                    TileUtils.CreateToastNotifications("创建成功");
                    CreateSecondTileNotifications(tileId, imageUri, tileName, subName);
                }
                else
                {
                    TileUtils.CreateToastNotifications("创建失败");
                }
            }
        }
    }
}
