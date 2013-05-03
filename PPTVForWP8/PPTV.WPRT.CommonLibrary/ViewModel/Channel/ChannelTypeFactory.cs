using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Channel
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.Utils;
    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel.Download;

    public class ChannelTypeFactory
    {
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
        //public const int LiveTypeId = -23;
        public static readonly ChannelTypeFactory Instance = new ChannelTypeFactory();

        public ObservableCollection<ChannelTypeItem> SelectedViewModel
        {
            get { return _selectedViewModel; }
        }

        public ChannelTypeViewModel AllViewModel
        {
            get { return _allViewModel; }
        }

        public ChannelTypeItem[] LocalTypes
        {
            get { return _localTypes; }
        }

        private ChannelTypeFactory()
        {
            _typeImages = new Dictionary<string, string>(19);
            _typeImages.Add("VIP尊享", "/Images/Channels/vip.png");
            _typeImages.Add("电影", "/Images/Channels/movie.png");
            _typeImages.Add("电视剧", "/Images/Channels/teleplay.png");
            _typeImages.Add("动漫", "/Images/Channels/cartoon.png");
            _typeImages.Add("综艺", "/Images/Channels/show.png");
            _typeImages.Add("体育", "/Images/Channels/sports.png");
            _typeImages.Add("热点", "/Images/Channels/hot.png");
            _typeImages.Add("游戏", "/Images/Channels/game.png");
            //_typeImages.Add("推荐分类", "/Images/Channels/recommend.png");
            _typeImages.Add("旅游", "/Images/Channels/travel.png");
            //_typeImages.Add("生活", "/Images/Channels/life.png");
            //_typeImages.Add("时尚", "/Images/Channels/modern.png");
            _typeImages.Add("音乐", "/Images/Channels/music.png");
            _typeImages.Add("娱乐", "/Images/Channels/flower.png");
            //_typeImages.Add("搞笑", "/Images/Channels/funny.png");
            _typeImages.Add("最近观看", "/Images/Channels/history.png");
            _typeImages.Add("我的收藏", "/Images/Channels/favoriten.png");
            _typeImages.Add("我的下载", "/Images/Channels/downloaded.png");
            //_typeImages.Add("直播", "/Images/Channels/live.png");

            _defaultImage = "/Images/Channels/default.png";
            _channelTypeSettingKey = "ChannelTypeSettingKey";
            _localTypes = new ChannelTypeItem[] {
                new ChannelTypeItem(){TypeId = RecentTypeId, TypeName="最近观看", ImageUri=_typeImages["最近观看"]},
                new ChannelTypeItem(){TypeId = FavoritenTypeId, TypeName="我的收藏", ImageUri=_typeImages["我的收藏"]},
                new ChannelTypeItem(){TypeId = DownloadedTypeId, TypeName="我的下载", ImageUri=_typeImages["我的下载"], Count = DownloadViewModel.Instance.DownloadingItems.Count}
                //new ChannelTypeItem(){TypeId = LiveTypeId, TypeName="直播", ImageUri = _defaultImage}
            };

            _allViewModel = new ChannelTypeViewModel();
            _selectedViewModel = new ObservableCollection<ChannelTypeItem>();

            var selectGroup = new ChannelTypeGroup();
            selectGroup.GroupName = "已固定";

            var unSelectGroup = new ChannelTypeGroup();
            unSelectGroup.GroupName = "未固定";

            _allViewModel.Add(selectGroup);
            _allViewModel.Add(unSelectGroup);

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
            _allViewModel[0].PerformanceClear();
            _allViewModel[1].PerformanceClear();
            _selectedViewModel.PerformanceClear();
        }

        public void Cancel()
        {
            _typeFactory.Cancel();
        }

        public void AddSelectedType(ChannelTypeItem channelType)
        {
            _selectedViewModel.Add(channelType);
            _allViewModel[0].Add(channelType);
            _allViewModel[1].Remove(channelType);
            SaveRoaming();
        }

        public void AddSelectedTypes(IList channelTypes)
        {
            var builder = new StringBuilder(10);
            while (channelTypes.Count > 0)
            {
                var channelType = channelTypes[0] as ChannelTypeItem;
                if (channelType != null)
                {
                    _selectedViewModel.Add(channelType);
                    _allViewModel[0].Add(channelType);
                    _allViewModel[1].Remove(channelType);
                    builder.AppendFormat("\"{0}\"", channelType.TypeName);
                }
            }
            SaveRoaming();
            TileUtils.CreateBasicToast(
                        string.Format("{0}被固定到首页", builder));
        }

        public void RemoveSelecedType(ChannelTypeItem channelType)
        {
            if (_selectedViewModel.Count > 1)
            {
                _selectedViewModel.Remove(channelType);
                _allViewModel[0].Remove(channelType);
                _allViewModel[1].Add(channelType);
                SaveRoaming();
                TileUtils.CreateBasicToast(
                        string.Format("首页已经移除了\"{0}\" 您在\"频道\"内可以找到它", channelType.TypeName));
            }
            else
            {
                TileUtils.CreateBasicToast("已经不能再移除了");
            }
        }

        public void RemoveSelectedTypes(IList channelTypes)
        {
            if (_selectedViewModel.Count > 1 
                && _selectedViewModel.Count > channelTypes.Count)
            {
                var builder = new StringBuilder(10);
                while (channelTypes.Count > 0)
                {
                    var channelType = channelTypes[0] as ChannelTypeItem;
                    if (channelType != null)
                    {
                        _selectedViewModel.Remove(channelType);
                        _allViewModel[0].Remove(channelType);
                        _allViewModel[1].Add(channelType);
                        builder.AppendFormat("\"{0}\"", channelType.TypeName);
                    }
                }
                SaveRoaming();
                TileUtils.CreateBasicToast(
                        string.Format("首页已经移除了{0}您在\"频道\"内可以找到它", builder));
            }
            else
            {
                TileUtils.CreateBasicToast("首页必须有一个以上的频道");
            }
        }

        private void typeFactory_HttpSucess(object sender, HttpFactoryArgs<List<TypeInfo>> args)
        {
            var roamingValue = DataStoreUtils.GetLocalSettingValue(_channelTypeSettingKey) as string;
            if (string.IsNullOrEmpty(roamingValue))
                roamingValue = string.Join("/", args.Result.Take(12).Select(v => v.TypeId.ToString()));
            var firstLaunch = DataStoreUtils.GetLocalSettingValue(_channelTypeSettingKey) as string;
            if (string.IsNullOrEmpty(firstLaunch))
            {
                if (!roamingValue.Contains("75099"))
                {
                    roamingValue = roamingValue.Insert(0, "75099/");
                    roamingValue = roamingValue.Remove(roamingValue.LastIndexOf('/'));
                }
                //if (!roamingValue.Contains(LiveTypeId.ToString()))
                //{
                //    roamingValue = roamingValue.Insert(0, string.Format("{0}/", LiveTypeId.ToString()));
                //    roamingValue = roamingValue.Remove(roamingValue.LastIndexOf('/'));
                //}
                DataStoreUtils.AddLocalSettingValue(_channelTypeSettingKey, "1");
                DataStoreUtils.AddLocalSettingValue(_channelTypeSettingKey, roamingValue);
            }

            var selectValues = roamingValue.Split('/');
            foreach (var type in args.Result)
            {
                ProcessViewModel(selectValues, type.TypeId, type.Name);
            }
            foreach (var type in _localTypes)
            {
                ProcessViewModel(selectValues, type.TypeId, type.TypeName);
            }
            foreach (var selected in selectValues)
            {
                var item = _allViewModel[0].FirstOrDefault(v => v.TypeId == Convert.ToInt32(selected));
                if (item != null)
                {
                    _selectedViewModel.Add(item);
                }
            }
        }

        private void SaveRoaming()
        {
            var roamingValue = string.Join("/", _selectedViewModel.Select(v => v.TypeId.ToString()));
            DataStoreUtils.AddLocalSettingValue(_channelTypeSettingKey, roamingValue);
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
                _allViewModel[0].Add(groupItem);
            }
            else
            {
                _allViewModel[1].Add(groupItem);
            }
        }

        #region For Channels

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
            var typeInfo = AllViewModel[0].FirstOrDefault(v => v.TypeId == typeId);
            if (typeInfo == null)
                typeInfo = AllViewModel[1].FirstOrDefault(v => v.TypeId == typeId);
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
            var result = AllViewModel[0].FirstOrDefault(v => v.TypeName == typeName);
            if (result == null)
                return AllViewModel[1].FirstOrDefault(v => v.TypeName == typeName);
            return result;
        }

        public string GetLocalPath(int typeId)
        {
            switch (typeId)
            { 
                case RecentTypeId:
                    return "/Pages/HistoryPage.xaml";
                case FavoritenTypeId:
                    return "/Pages/FavoritenPage.xaml";
                case DownloadedTypeId:
                    return "/Pages/DownloadPage.xaml";
                default:
                    return string.Empty;
            }
        }

        #endregion
    }
}
