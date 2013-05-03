using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData
{
    using PPTVData.Entity;
    using PPTVData.Entity.Live;

    public static class ExtensionClass
    {
        #region Enum CreateString

        public static string CreateString(this PlayFileType fileType)
        {
            switch (fileType)
            { 
                case PlayFileType.Smooth:
                    return "流畅";
                case PlayFileType.HD:
                    return "高清";
                case PlayFileType.SuperHD:
                    return "超清";
                default:
                    return string.Empty;
            }
        }

        public static string CreateString(this TagDimension dimension)
        {
            switch (dimension)
            { 
                case TagDimension.Catalog:
                    return "catalog";
                case TagDimension.Area:
                    return "area";
                case TagDimension.Year:
                    return "year";
                default:
                    return string.Empty;
            }
        }

        public static string CreateString(this LiveType type)
        {
            switch (type)
            { 
                case LiveType.Recommend:
                    return "推荐";
                case LiveType.Satellite:
                    return "卫视";
                case LiveType.LocalTV:
                    return "地方台";
                case LiveType.Sports:
                    return "体育";
                case LiveType.Games:
                    return "游戏";
                default:
                    return string.Empty;
            }
        }

        #endregion

        /// <summary>
        /// 由于目前直接调用clear会导致Ui卡死，逐个移除却不会
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="list"></param>
        public static void PerformanceClear<T>(this ObservableCollection<T> list)
        {
            var count = list.Count;
            for (; count > 0; count--)
            {
                list.RemoveAt(0);
            }
        }
    }
}
