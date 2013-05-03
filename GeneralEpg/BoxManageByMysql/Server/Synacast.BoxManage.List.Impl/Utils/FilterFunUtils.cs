using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Impl.Utils
{
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;

    public static class FilterFunUtils
    {
        #region Vod

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> BeginLetter = (v, pars, filter) => v.SpellName.Contains(filter.beginletter);

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Flags = (v, pars, filter) => pars.Flags.Except(v.Flags).Count() == 0;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Tags = (v, pars, filter) => pars.Tags.Except(v.TagsRank.Keys).Count() == 0;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Ver1 = (v, pars, filter) => v.Main.TableType == 0;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Ver2 = (v, pars, filter) => !v.IsGroup;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Virtual = (v, pars, filter) => v.Main.TableType != 1;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> ForbidVip = (v, pars, filter) => v.Main.VipFlag == 0;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> BitMin = (v, pars, filter) => v.Main.BitRate > filter.bitratemin;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> BitMax = (v, pars, filter) => v.Main.BitRate <= filter.bitratemax;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> WMin = (v, pars, filter) => v.Main.VideoWidth > filter.wmin;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> WMax = (v, pars, filter) => v.Main.VideoWidth <= filter.wmax;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> HMin = (v, pars, filter) => v.Main.VideoHeight > filter.hmin;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> HMax = (v, pars, filter) => v.Main.VideoHeight <= filter.hmax;

        public static readonly Func<VideoNode, VideoPars, ListFilter, bool> Forbidden = (v, pars, filter) => !v.ForbiddenAreas.Contains(pars.ForbiddenName);
        
        #endregion

        #region Live

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveHiddenFlag = (v, pars, filter) => v.HiddenFlag == 0;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveBeginLetter = (v, pars, filter) => v.SpellName.Contains(filter.beginletter);

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveFlags = (v, pars, filter) => pars.Flags.Except(v.Flags).Count() == 0;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveVer1 = (v, pars, filter) => v.Main.TableType == 0;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveVer2 = (v, pars, filter) => !v.IsGroup;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveForbidVip = (v, pars, filter) => v.Main.VipFlag == 0;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveBitMin = (v, pars, filter) => v.Main.BitRate > filter.bitratemin;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveBitMax = (v, pars, filter) => v.Main.BitRate <= filter.bitratemax;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveWMin = (v, pars, filter) => v.Main.VideoWidth > filter.wmin;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveWMax = (v, pars, filter) => v.Main.VideoWidth <= filter.wmax;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveHMin = (v, pars, filter) => v.Main.VideoHeight > filter.hmin;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveHMax = (v, pars, filter) => v.Main.VideoHeight <= filter.hmax;

        public static readonly Func<LiveVideoNode, VideoPars, LiveFilter, bool> LiveForbidden = (v, pars, filter) => !v.ForbiddenAreas.Contains(pars.ForbiddenName);

        #endregion

        #region Common

        public static readonly Func<VideoBase, ExFilterBase, bool> ComVer1 = (v, filter) => v.Main.TableType == 0;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComVer2 = (v, filter) => !v.IsGroup;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComVirtual = (v, filter) => v.Main.TableType != 1;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComForbidVip = (v, filter) => v.Main.VipFlag == 0;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComBitMin = (v, filter) => v.Main.BitRate > filter.bitratemin;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComBitMax = (v, filter) => v.Main.BitRate <= filter.bitratemax;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComWMin = (v, filter) => v.Main.VideoWidth > filter.wmin;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComWMax = (v, filter) => v.Main.VideoWidth <= filter.wmax;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComHMin = (v, filter) => v.Main.VideoHeight > filter.hmin;

        public static readonly Func<VideoBase, ExFilterBase, bool> ComHMax = (v, filter) => v.Main.VideoHeight <= filter.hmax;

        public static readonly Func<VideoBase, ExFilterBase, bool> Common = (v, filter) => v.Main.BitRate > filter.bitratemin && v.Main.BitRate <= filter.bitratemax && v.Main.VideoWidth > filter.wmin && v.Main.VideoWidth <= filter.wmax && v.Main.VideoHeight > filter.hmin && v.Main.VideoHeight <= filter.hmax;

        public static readonly Func<VideoBase, ExFilterBase, bool> CommonForbidden = (v, filter) => !v.ForbiddenAreas.Contains(filter.commonfbcode);

        #endregion
    }
}
