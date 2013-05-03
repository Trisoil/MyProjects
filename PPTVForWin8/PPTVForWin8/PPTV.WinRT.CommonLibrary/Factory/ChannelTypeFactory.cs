using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Factory
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    public class ChannelTypeFactory
    {
        private bool _isReorder;
        private string _defaultImage;
        private string _channelTypeSettingKey;

        private ChannelTypeItem[] _localTypes;
        private Dictionary<string, string> _typeImages;
        private ObservableCollection<ChannelTypeItem> _selectedViewModel;
        private ChannelTypeViewModel _allViewModel;
        private TypeFactory _typeFactory;

        public Action HttpFailorTimeOut;
        public const int RecentTypeId = -20;
        public const int FavoritenTypeId = -21;
        public const int DownloadedTypeId = -22;
        public const int LiveTypeId = -23;
        public static readonly ChannelTypeFactory Instance = new ChannelTypeFactory();

        public ObservableCollection<ChannelTypeItem> SelectedViewModel
        {
            get { return _selectedViewModel; }
        }

        public ChannelTypeViewModel AllViewModel
        {
            get { return _allViewModel; }
        }

        public int ColNumber { get; set; }

        private ChannelTypeFactory()
        {
            _typeImages = new Dictionary<string, string>(19);
            _typeImages.Add("VIP尊享", "ms-appx:///Images/Channels/vip.png");
            _typeImages.Add("电影", "ms-appx:///Images/Channels/movie.png");
            _typeImages.Add("电视剧", "ms-appx:///Images/Channels/teleplay.png");
            _typeImages.Add("动漫", "ms-appx:///Images/Channels/cartoon.png");
            _typeImages.Add("综艺", "ms-appx:///Images/Channels/show.png");
            _typeImages.Add("体育", "ms-appx:///Images/Channels/sports.png");
            _typeImages.Add("热点", "ms-appx:///Images/Channels/hot.png");
            _typeImages.Add("游戏", "ms-appx:///Images/Channels/game.png");
            _typeImages.Add("推荐分类", "ms-appx:///Images/Channels/recommend.png");
            _typeImages.Add("旅游", "ms-appx:///Images/Channels/travel.png");
            _typeImages.Add("生活", "ms-appx:///Images/Channels/life.png");
            _typeImages.Add("时尚", "ms-appx:///Images/Channels/modern.png");
            _typeImages.Add("音乐", "ms-appx:///Images/Channels/music.png");
            _typeImages.Add("娱乐", "ms-appx:///Images/Channels/flower.png");
            _typeImages.Add("搞笑", "ms-appx:///Images/Channels/funny.png");
            _typeImages.Add("最近观看", "ms-appx:///Images/Channels/history.png");
            _typeImages.Add("我的收藏", "ms-appx:///Images/Channels/favoriten.png");
            _typeImages.Add("已下载", "ms-appx:///Images/Channels/downed.png");
            _typeImages.Add("直播", "ms-appx:///Images/Channels/live.png");

            _defaultImage = "ms-appx:///Images/Channels/default.png";
            _channelTypeSettingKey = "ChannelTypeSettingKey";
            _localTypes = new ChannelTypeItem[] {
                new ChannelTypeItem(){TypeId = RecentTypeId, TypeName="最近观看", ImageUri=_defaultImage},
                new ChannelTypeItem(){TypeId = FavoritenTypeId, TypeName="我的收藏", ImageUri=_defaultImage},
                new ChannelTypeItem(){TypeId = DownloadedTypeId, TypeName="已下载", ImageUri=_defaultImage},
                new ChannelTypeItem(){TypeId = LiveTypeId, TypeName="直播", ImageUri = _defaultImage}
            };

            _allViewModel = new ChannelTypeViewModel();
            _allViewModel.Groups = new ObservableCollection<ChannelTypeGroup>();

            _selectedViewModel = new ObservableCollection<ChannelTypeItem>();
            _selectedViewModel.CollectionChanged += selectedTypes_CollectionChanged;

            _typeFactory = new TypeFactory();
            _typeFactory.HttpSucessHandler += typeFactory_HttpSucess;
            _typeFactory.HttpFailorTimeOut += HttpFailorTimeOut;
        }

        public void DownloadDatas()
        {
            _typeFactory.DownLoadDatas();
        }

        public void Clear()
        {
            _allViewModel.Groups.PerformanceClear();
            _selectedViewModel.PerformanceClear();
            _isReorder = false;
        }

        public void Cancel()
        {
            _typeFactory.Cancel();
        }

        public void AddSelectedType(ChannelTypeItem channelType)
        {
            _selectedViewModel.Add(channelType);
            _allViewModel.Groups[0].ItemsGroup.Add(channelType);
            _allViewModel.Groups[1].ItemsGroup.Remove(channelType);
            SaveRoaming();
        }

        public void RemoveSelecedType(ChannelTypeItem channelType)
        {
            if (_selectedViewModel.Count > 1)
            {
                _selectedViewModel.Remove(channelType);
                _allViewModel.Groups[0].ItemsGroup.Remove(channelType);
                _allViewModel.Groups[1].ItemsGroup.Add(channelType);
                SaveRoaming();
                TileUtils.CreateToastNotificationsText02(
                        string.Format("首页已经移除了\"{0}\"", channelType.TypeName), "您在\"频道\"内可以找到它");
            }
            else
            {
                TileUtils.CreateToastNotifications("已经不能再移除了");
            }
        }

        private void selectedTypes_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == NotifyCollectionChangedAction.Remove)
            {
                _isReorder = true;
            }
            if (e.Action == NotifyCollectionChangedAction.Add
                && _isReorder)
            {
                SaveRoaming();
                _isReorder = false;
            }
        }

        private void typeFactory_HttpSucess(object sender, HttpFactoryArgs<List<TypeInfo>> args)
        {
            var selectGroup = new ChannelTypeGroup();
            selectGroup.GroupName = "已固定";
            selectGroup.ItemsGroup = new ObservableCollection<ChannelTypeItem>();

            var unSelectGroup = new ChannelTypeGroup();
            unSelectGroup.GroupName = "未固定";
            unSelectGroup.ItemsGroup = new ObservableCollection<ChannelTypeItem>();

            _allViewModel.Groups.Add(selectGroup);
            _allViewModel.Groups.Add(unSelectGroup);

            var roamingValue = DataStoreUtils.GetRoamingSettingValue(_channelTypeSettingKey) as string;
            if (string.IsNullOrEmpty(roamingValue))
                roamingValue = string.Join("/", args.Result.Take(ColNumber * 2).Select(v => v.TypeId.ToString()));
            var firstLaunch = DataStoreUtils.GetLocalSettingValue(_channelTypeSettingKey) as string;
            if (string.IsNullOrEmpty(firstLaunch))
            {
                if (!roamingValue.Contains("75099"))
                {
                    roamingValue = roamingValue.Insert(0, "75099/");
                    roamingValue = roamingValue.Remove(roamingValue.LastIndexOf('/'));
                }
                if (!roamingValue.Contains(LiveTypeId.ToString()))
                {
                    roamingValue = roamingValue.Insert(0, string.Format("{0}/", LiveTypeId.ToString()));
                    roamingValue = roamingValue.Remove(roamingValue.LastIndexOf('/'));
                }
                DataStoreUtils.AddLocalSettingValue(_channelTypeSettingKey, "1");
                DataStoreUtils.AddRoamingSettingValue(_channelTypeSettingKey, roamingValue);
            }

            _isReorder = false;

            var selectValues = roamingValue.Split('/');
            foreach (var type in args.Result)
            {
                ProcessViewModel(selectValues, type.TypeId, type.Name);
            }
            foreach (var type in _localTypes)
            {
                ProcessViewModel(selectValues, type.TypeId, type.TypeName);
            }
            foreach(var selected in selectValues)
            {
                var item = _allViewModel.Groups[0].ItemsGroup.FirstOrDefault(v => v.TypeId == Convert.ToInt32(selected));
                if (item != null)
                {
                    _selectedViewModel.Add(item);
                }
            }
        }

        private void SaveRoaming()
        {
            var roamingValue = string.Join("/",_selectedViewModel.Select(v => v.TypeId.ToString()));
            DataStoreUtils.AddRoamingSettingValue(_channelTypeSettingKey, roamingValue);
        }

        private void ProcessViewModel(string[] roamingValues, int typeId, string typeName)
        {
            var groupItem = new ChannelTypeItem()
            {
                TypeId = typeId,
                TypeName = typeName
            };

            if (_typeImages.ContainsKey(groupItem.TypeName))
                groupItem.ImageUri = _typeImages[groupItem.TypeName];
            else
                groupItem.ImageUri = _defaultImage;

            if (roamingValues.Contains(typeId.ToString()))
            {
                _allViewModel.Groups[0].ItemsGroup.Add(groupItem);
            }
            else
            {
                _allViewModel.Groups[1].ItemsGroup.Add(groupItem);
            }
        }

        #region For Channels

        public int GetItemWidth(string typeName)
        {
            if (JudgeLandscapeChannel(typeName))
                return 160;
            return 120;
        }

        public int GetItemHeight(string typeName)
        {
            if (JudgeLandscapeChannel(typeName))
                return 120;
            return 160;
        }

        public string GetItemImage(string typeName, ChannelInfo chanleInfo)
        {
            if (JudgeLandscapeChannel(typeName))
                return chanleInfo.SlotUri;
            return chanleInfo.ImageUri;
        }

        public bool JudgeVipChannel(string typeName)
        {
            return typeName == "VIP尊享";
        }

        public bool JudgeLandscapeChannel(string typeName)
        {
            return typeName == "体育"
                || typeName == "游戏" || typeName == "热点" || typeName == "搞笑" || typeName == "时尚";
        }

        public bool JudgeLandscapeChannel(int typeId)
        {
            var typeInfo = AllViewModel.Groups[0].ItemsGroup.FirstOrDefault(v => v.TypeId == typeId);
            if (typeInfo == null)
                typeInfo = AllViewModel.Groups[1].ItemsGroup.FirstOrDefault(v => v.TypeId == typeId);
            if (typeInfo != null)
                return JudgeLandscapeChannel(typeInfo.TypeName);
            return false;
        }

        public int GetChannelTypeId(string typeName)
        {
            var typeInfo = GetChannelTypeItem(typeName);
            if (typeInfo != null)
                return typeInfo.TypeId;
            return 0;
        }

        public ChannelTypeItem GetChannelTypeItem(string typeName)
        {
            var result = AllViewModel.Groups[0].ItemsGroup.FirstOrDefault(v => v.TypeName == typeName);
            if (result == null)
                return AllViewModel.Groups[1].ItemsGroup.FirstOrDefault(v => v.TypeName == typeName);
            return result;
        }

        #endregion
    }
}
