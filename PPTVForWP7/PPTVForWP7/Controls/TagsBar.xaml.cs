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

using PPTVData.Entity;
using PPTVData.Factory;
using System.Threading;
using System.ComponentModel;
using System.Diagnostics;
using PPTVData.Log;

namespace PPTVForWP7.Controls
{
    public partial class TagsBar : UserControl
    {
        public event EventHandler DataDownloadComplete;
        public event EventHandler DataDownloadFailed;

        public delegate void TagSelectEventHandler(object sender, List<TagInfo> tagInfos);
        public event TagSelectEventHandler TagChanged;
        private List<TagInfo> _currentSelectedTagInfos = new List<TagInfo> { new TagInfo(1, "catalog", "全部", -1), new TagInfo(1, "year", "全部", -1), new TagInfo(1, "area", "全部", -1) };//当前选中的Tag
        public List<TagInfo> CurrentSelectedTagInfos
        {
            get { return _currentSelectedTagInfos; }
            set
            {
                _currentSelectedTagInfos = value;
                SetTagSelected();
            }
        }

        BackgroundWorker _backgroundWorker = new BackgroundWorker();

        TagFactory _tagFactoryOfCatalog = new TagFactory();
        TagFactory _tagFactoryOfArea = new TagFactory();
        TagFactory _tagFactoryOfYear = new TagFactory();

        List<TagInfo> _tagInfosOfCatalog = new List<TagInfo>();
        List<TagInfo> _tagInfosOfArea = new List<TagInfo>();
        List<TagInfo> _tagInfosOfYear = new List<TagInfo>();
        private int _typeID = -1;
        public int TypeID
        {
            get { return _typeID; }
            set
            {
                _typeID = value;
                StartInitData();
            }
        }

        private bool _isDownloadedData = false;
        public bool IsDownloadedData
        {
            get { return _isDownloadedData; }
            private set
            {
                _isDownloadedData = value;
                if (_isDownloadedData)
                {
                    if (DataDownloadComplete != null)
                        DataDownloadComplete(this, null);
                }
            }
        }

        private void SetTagSelected()
        {
            foreach (Button button in xCatalogListBox.Items)
            {
                string tagName = (button.Tag as TagInfo).TagName;
                if (tagName == _currentSelectedTagInfos[0].TagName)
                    button.Background = new SolidColorBrush(Utils.Utils.GetColor("#FF00AEFF"));
                else
                    button.Background = new SolidColorBrush(Colors.White);
            }
            foreach (Button button in xAreaListBox.Items)
            {
                string tagName = (button.Tag as TagInfo).TagName;
                if (tagName == _currentSelectedTagInfos[1].TagName)
                    button.Background = new SolidColorBrush(Utils.Utils.GetColor("#FF00AEFF"));
                else
                    button.Background = new SolidColorBrush(Colors.White);
            }
            foreach (Button button in xYearListBox.Items)
            {
                string tagName = (button.Tag as TagInfo).TagName;
                if (tagName == _currentSelectedTagInfos[2].TagName)
                    button.Background = new SolidColorBrush(Utils.Utils.GetColor("#FF00AEFF"));
                else
                    button.Background = new SolidColorBrush(Colors.White);
            }
            LogManager.Ins.Log("设置选中的Tag");
        }

        private void StartInitData()
        {
            if (_typeID == -1)
                return;
            SetDownloadStatus(true);
            _tagFactoryOfYear.DownLoadDatas(_typeID, "year");
            _tagFactoryOfArea.DownLoadDatas(_typeID, "area");
            _tagFactoryOfCatalog.DownLoadDatas(_typeID, "catalog");
            LogManager.Ins.Log("开始下载Tag数据");
        }

        public TagsBar()
        {
            InitializeComponent();
            InitVariableNEventHandle();
            Loaded += TagsBar_Loaded;
        }

        void TagsBar_Loaded(object sender, RoutedEventArgs e)
        {
            LogManager.Ins.Log("TagsBar_Loaded");
        }

        void InitElement(ListBox listBox, List<TagInfo> tagInfos)
        {
            //!!!貌似不起作用
            LogManager.Ins.Log("开始加载数据Tag到UI");
            Thread thread = new Thread(new ThreadStart(() =>
            {
                Dispatcher.BeginInvoke(() =>
                {
                    listBox.Items.Clear();
                    foreach (TagInfo item in tagInfos)
                    {
                        Button button = new Button();
                        button.Style = Application.Current.Resources["TagsButtonStyle"] as Style;
                        button.Foreground = new SolidColorBrush(Colors.White);
                        button.Background = new SolidColorBrush(Colors.White);
                        button.Height = 35;
                        button.Margin = new Thickness(0, 20, 0, 0);
                        button.Content = item.TagName;
                        button.Tag = item;
                        button.Tap += new EventHandler<GestureEventArgs>(button_Tap);
                        if (button.Content.ToString() == "全部")
                            button.Background = new SolidColorBrush(Utils.Utils.GetColor("#FF00AEFF"));
                        listBox.Items.Add(button);
                    }

                    if (listBox.Equals(xCatalogListBox))
                    {
                        xCatalogProgressBar.Visibility = Visibility.Collapsed;
                    }
                    else if (listBox.Equals(xAreaListBox))
                    {
                        xAreaProgressBar.Visibility = Visibility.Collapsed;
                    }
                    else if (listBox.Equals(xYearListBox))
                    {
                        xYearProgressBar.Visibility = Visibility.Collapsed;
                    }
                }
                );
            }));
            thread.IsBackground = true;
            thread.Start();
            LogManager.Ins.Log("加载完数据Tag到UI");
        }

        void button_Tap(object sender, GestureEventArgs e)
        {
            Button button = sender as Button;
            TagInfo tagInfo = button.Tag as TagInfo;

            if (tagInfo.Dimension == "catalog")
            {
                if (tagInfo.TagName.Equals(_currentSelectedTagInfos[0].TagName))
                    return;
                _currentSelectedTagInfos[0] = tagInfo;
            }
            if (tagInfo.Dimension == "area")
            {
                if (tagInfo.TagName.Equals(_currentSelectedTagInfos[1].TagName))
                    return;
                _currentSelectedTagInfos[1] = tagInfo;
            }
            if (tagInfo.Dimension == "year")
            {
                if (tagInfo.TagName.Equals(_currentSelectedTagInfos[2].TagName))
                    return;
                _currentSelectedTagInfos[2] = tagInfo;
            }
            if (TagChanged != null)
                TagChanged(this, _currentSelectedTagInfos);
        }

        #region 获取数据处理函数
        private void InitVariableNEventHandle()
        {
            _tagFactoryOfCatalog.HttpFailHandler += _tagFactoryOfCatalog_GetTagsFailed;
            _tagFactoryOfCatalog.HttpSucessHandler += _tagFactoryOfCatalog_GetTagsSucceed;
            _tagFactoryOfCatalog.HttpTimeOutHandler += _tagFactoryOfCatalog_GetTagsTimeout;

            _tagFactoryOfArea.HttpFailHandler += _tagFactoryOfArea_GetTagsFailed;
            _tagFactoryOfArea.HttpSucessHandler += _tagFactoryOfArea_GetTagsSucceed;
            _tagFactoryOfArea.HttpTimeOutHandler += _tagFactoryOfArea_GetTagsTimeout;

            _tagFactoryOfYear.HttpFailHandler += _tagFactoryOfYear_GetTagsFailed;
            _tagFactoryOfYear.HttpSucessHandler += _tagFactoryOfYear_GetTagsSucceed;
            _tagFactoryOfYear.HttpTimeOutHandler += _tagFactoryOfYear_GetTagsTimeout;
        }

        void _tagFactoryOfYear_GetTagsTimeout(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xYearProgressBar.Visibility = Visibility.Collapsed;
        }

        void _tagFactoryOfArea_GetTagsTimeout(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xAreaProgressBar.Visibility = Visibility.Collapsed;
        }

        void _tagFactoryOfCatalog_GetTagsTimeout(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xCatalogProgressBar.Visibility = Visibility.Collapsed;
        }

        void _tagFactoryOfYear_GetTagsSucceed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            LogManager.Ins.Log("下载守Year数据Tag");
            SetDownloadStatus(false);
            _tagInfosOfYear = args.Result.Tags;
            InitElement(xYearListBox, _tagInfosOfYear);
        }

        void _tagFactoryOfYear_GetTagsFailed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xYearProgressBar.Visibility = Visibility.Collapsed;
            if (DataDownloadFailed != null)
                DataDownloadFailed(sender, null);
        }

        void _tagFactoryOfArea_GetTagsSucceed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            LogManager.Ins.Log("下载守Aear数据Tag");
            SetDownloadStatus(false);
            _tagInfosOfArea = args.Result.Tags;

            InitElement(xAreaListBox, _tagInfosOfArea);
        }

        void _tagFactoryOfArea_GetTagsFailed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xAreaProgressBar.Visibility = Visibility.Collapsed;
            if (DataDownloadFailed != null)
                DataDownloadFailed(sender, null);
        }

        void _tagFactoryOfCatalog_GetTagsSucceed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            LogManager.Ins.Log("下载守Catalog数据Tag");
            SetDownloadStatus(false);
            _tagInfosOfCatalog = args.Result.Tags;
            InitElement(xCatalogListBox, _tagInfosOfCatalog);
        }

        void _tagFactoryOfCatalog_GetTagsFailed(object sender, HttpFactoryArgs<TagFactoryInfo> args)
        {
            xCatalogProgressBar.Visibility = Visibility.Collapsed;
            if (DataDownloadFailed != null)
                DataDownloadFailed(sender, null);
        }
        #endregion

        /// <summary>
        /// isBeginDownload==true 表示 正在开始下载数据
        /// isBeginDownload==false 表示 对数据下载完时的处理方法
        /// </summary>
        /// <param name="isBeginDownload"></param>
        void SetDownloadStatus(bool isBeginDownload)
        {
            if (isBeginDownload)
            {
                IsDownloadedData = false;
                _tagInfosOfCatalog.Clear();
                _tagInfosOfArea.Clear();
                _tagInfosOfYear.Clear();
                xCatalogProgressBar.Visibility = xAreaProgressBar.Visibility = xYearProgressBar.Visibility = Visibility.Visible;
            }
            else
            {
                if (_tagInfosOfCatalog.Count != 0 && _tagInfosOfArea.Count != 0 && _tagInfosOfYear.Count != 0)
                {
                    IsDownloadedData = true;
                }
            }
        }
    }
}
