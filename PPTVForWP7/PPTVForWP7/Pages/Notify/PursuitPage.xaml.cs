using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;

namespace PPTVForWP7.Pages.Notify
{
    using PPTVForWP7.Utils;
    using PPTVForWP7.Controls;
    using PPTVData.Entity.Notify;

    public partial class PursuitPage : PhoneApplicationPage
    {
        /// <summary>
        /// 是否操作
        /// </summary>
        bool _operateCheck = true;

        static PursuitPage()
        {
            TiltEffect.TiltableItems.Add(typeof(PursuitItem));
        }

        public PursuitPage()
        {
            InitializeComponent();
            xTapNotify.IsChecked = PursuitUtils.JudgePursuit();
            this.Loaded += new RoutedEventHandler(PursuitPage_Loaded);
            InitPrusuitInfos();
        }

        void PursuitPage_Loaded(object sender, RoutedEventArgs e)
        {
            xTapNotify.Checked += xTapNotify_Checked;
        }

        /// <summary>
        /// 加载追剧信息
        /// </summary>
        private void InitPrusuitInfos()
        {
            xPursuitListBox.ItemsSource = PursuitViewModel.Instance.DataInfos;
            foreach (var pursuit in PursuitViewModel.Instance.DataInfos)
            {
                if (pursuit.IsShowDelte)
                    pursuit.IsShowDelte = false;
            }
        }

        /// <summary>
        /// 编辑
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ApplicationBarEdit_Click(object sender, EventArgs e)
        {
            foreach (var pursuit in PursuitViewModel.Instance.DataInfos)
            {
                pursuit.IsShowDelte = pursuit.IsShowDelte ? false : true;
            }
        }

        /// <summary>
        /// 打开追剧
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xTapNotify_Checked(object sender, RoutedEventArgs e)
        {
            if (!_operateCheck)
                return;
            var isInit = string.IsNullOrEmpty(
               Utils.GetIOSData(PursuitUtils.HttpNotifyChannelName, string.Empty)) ? false : true;
            if (!isInit)
                PursuitUtils.InitHttpNotify();
            if (PursuitViewModel.Instance.DataInfos.Count > 0)
            {
                Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "1");
                xLoadingWait.Visibility = Visibility.Visible;
                OperatePursuit(0, PursuitViewModel.Instance.DataInfos[0], true, false, result =>
                {
                    xLoadingWait.Visibility = Visibility.Collapsed;
                    var response = string.Empty;
                    if (result)
                    {
                        Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "1");
                        response = "打开追剧完成";
                    }
                    else
                    {
                        Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "0");
                        _operateCheck = false;
                        xTapNotify.IsChecked = false;
                        _operateCheck = true;
                        response = "打开追剧未完成，请检查网络";
                    }
                    xPopDialog.ContentText = response;
                    xPopDialog.IsShowEnable = true;
                });
            }
            else
            {
                Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "1");
            }
        }

        /// <summary>
        /// 关闭追剧
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xTapNotify_Unchecked(object sender, RoutedEventArgs e)
        {
            if (!_operateCheck)
                return;
            if (PursuitViewModel.Instance.DataInfos.Count > 0)
            {
                xLoadingWait.Visibility = Visibility.Visible;
                OperatePursuit(0, PursuitViewModel.Instance.DataInfos[0], false, false, result =>
                {
                    xLoadingWait.Visibility = Visibility.Collapsed;
                    var response = string.Empty;
                    if (result)
                    {
                        response = "关闭追剧完成";
                        //PursuitUtils.CloseHttpNotify();
                        Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "0");
                    }
                    else
                    {
                        _operateCheck = false;
                        xTapNotify.IsChecked = true;
                        _operateCheck = true;
                        response = "关闭追剧未完成，请检查网络";
                    }
                    xPopDialog.ContentText = response;
                    xPopDialog.IsShowEnable = true;
                });
            }
            else
            {
                Utils.SetIOSData(PursuitUtils.ISHttpNotifyName, "0");
            }
        }

        /// <summary>
        /// 删除追剧
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PursuitItem_PursuitDeleteEvent(object sender, RoutedEventArgs e)
        {
            var pursuit = sender as PursuitItem;
            if (pursuit != null)
            {
                var channelId = pursuit.ChannelId;
                xLoadingWait.Visibility = Visibility.Visible;
                PursuitUtils.PursuitChannel(channelId, false, (o, ee) =>
                {
                    var response = ee.IsSucess ? "删除成功" : "删除未成功，请检查网络";
                    Dispatcher.BeginInvoke(() => {
                        if (ee.IsSucess)
                            PursuitViewModel.Instance.RemoveByChannelId(channelId);
                        xLoadingWait.Visibility = Visibility.Collapsed;
                        xPopDialog.ContentText = response;
                        xPopDialog.IsShowEnable = true;
                    });
                });
            }
        }

        /// <summary>
        /// 全部清除
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ApplicationBarClear_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(PursuitUtils.RemovePursuitAlert,
                "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                if (PursuitViewModel.Instance.DataInfos.Count > 0)
                {
                    xLoadingWait.Visibility = Visibility.Visible;
                    var first = xPursuitListBox.Items[0] as PursuitInfo;
                    OperatePursuit(0, first, false, true, result =>
                    {
                        xLoadingWait.Visibility = Visibility.Collapsed;
                        var response = result ? "清空追剧完成" : "清空追剧未完成，请检查网络";
                        xPopDialog.ContentText = response;
                        xPopDialog.IsShowEnable = true;
                    });
                }
            }
        }

        /// <summary>
        /// 递归操作所有追剧
        /// </summary>
        /// <param name="pursuit"></param>
        /// <param name="callBack"></param>
        private void OperatePursuit(int index, PursuitInfo pursuit, bool isSub, bool isDeleted, Action<bool> callBack)
        {
            var channelId = pursuit.ChannelId;
            PursuitUtils.PursuitChannel(channelId, isSub, (o, ee) =>
            {
                Dispatcher.BeginInvoke(() =>
                {
                    if (ee.IsSucess)
                    {
                        if (isDeleted)
                        {
                            PursuitViewModel.Instance.RemoveEntity(pursuit);
                            if (xPursuitListBox.Items.Count > 0)
                            {
                                var p = xPursuitListBox.Items[0] as PursuitInfo;
                                OperatePursuit(0, p, isSub, isDeleted, callBack);
                            }
                            else
                            {
                                callBack(true);
                            }
                        }
                        else
                        {
                            index++;
                            if (index < xPursuitListBox.Items.Count)
                            {
                                var p = xPursuitListBox.Items[index] as PursuitInfo;
                                OperatePursuit(index, p, isSub, isDeleted, callBack);
                            }
                            else
                            {
                                callBack(true);
                            }
                        }
                    }
                    else
                    {
                        callBack(false);
                    }
                });
            });
        }

        /// <summary>
        /// 单击追剧查看详情
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PursuitItem_PursuitTapEvent(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var pursuit = sender as PursuitItem;
            if (pursuit != null)
            {
                var channelId = pursuit.ChannelId;
                if (pursuit.NewCount > pursuit.ChannelCount)
                {
                    PursuitViewModel.Instance.UpdateCountByChannelId(channelId, pursuit.NewCount);
                }
                NavigationService.Navigate(new Uri(string.Format(PursuitUtils.PursuitToDetailUri, channelId), UriKind.Relative));
            }
        }
    }
}