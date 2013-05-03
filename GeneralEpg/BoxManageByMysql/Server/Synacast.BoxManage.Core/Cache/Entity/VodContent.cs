using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Custom;

    /// <summary>
    /// 点播缓存主要内容
    /// </summary>
    [Serializable]
    public class VodContent
    {
        private List<int> _group;
        private List<int> _virtuals;
        private List<int> All;
        //private Dictionary<string, IEnumerable<int>> _dic;
        private List<int> _orderarray;

        public VodContent()
        {
            _group = new List<int>(100);
            _virtuals = new List<int>(100);
            All = new List<int>(2000);
        }

        public void AddVideo(VideoNode video)
        {
            if (video.Main.TableType == 0)
            {
                All.Add(video.Main.ChannelID);
            }
            else if (video.Main.TableType == 1)
            {
                All.Add(video.Main.ChannelID);
                _virtuals.Add(video.Main.ChannelID);
            }
            else
            {
                All.Add(video.Main.ChannelID);
                _group.Add(video.Main.ChannelID);

            }
        }


        public void Complete(Dictionary<int, VideoNode> videos)
        {
            var errors = new List<int>(10);
            foreach (var g in _group)   // 过滤组频道中不存在的频道
            {
                var channel = videos[g];
                int state = 0; int statecount = 0;
                errors.Clear();
                foreach (var gr in channel.Groups)
                {
                    VideoNode v;
                    if (videos.TryGetValue(gr, out v))
                    {
                        All.Remove(gr);
                        v.IsGroup = true;
                        state = state < v.State ? v.State : state;
                        statecount = statecount < v.StateCount ? v.StateCount : statecount;
                    }
                    else
                        errors.Add(gr);
                }
                foreach (var e in errors)
                {
                    channel.Groups.Remove(e);
                }
                if (channel.Groups.Count <= 0)
                {
                    All.Remove(channel.Main.ChannelID);
                }
                channel.State = state;
                channel.StateCount = statecount;
            }
            foreach (var v in _virtuals)  //过滤虚拟频道中不存在的频道
            {
                errors.Clear();
                var channel = videos[v];
                foreach (var vv in channel.Virtuals)
                {
                    if (!videos.ContainsKey(vv))
                        errors.Add(vv);
                }
                foreach (var e in errors)
                {
                    channel.Virtuals.Remove(e);
                }
                if (channel.Virtuals.Count > 0)
                {
                    channel.Groups[0] = channel.Virtuals[0];
                    channel.State = videos[channel.Groups[0]].State;
                    channel.StateCount = videos[channel.Groups[0]].StateCount;
                }
                else
                {
                    All.Remove(channel.Main.ChannelID);
                }
            }
            _orderarray = All;
            //_dic = new Dictionary<string, IEnumerable<int>>(1);
            //_dic["s"] = All;
            //_dic["t"] = All.OrderByDescending(v => videos[v].Hot);
            //_dic["n"] = All.OrderByDescending(v => videos[v].Main.ModifyTime);
            //_dic["a"] = All.OrderBy(v => videos[v].ChannelName);
            //_dic["g"] = All.OrderByDescending(v => videos[v].BKInfo.Score);
        }

        public List<int> OrderArray
        {
            get
            {
                return _orderarray;
            }
        }

        //public IEnumerable<int> GetLimitArray(string order)
        //{
        //    var result = _dic[order];
        //    if (result is List<int>)
        //        return result as List<int>;
        //    var list = result.ToPerfectList();
        //    _dic[order] = list;
        //    return list;
        //}

        

        public IEnumerable<int> Ver1
        { 
            get{
                return _orderarray.Where(v => ListCache.Instance.Dictionary[v].Main.TableType == 0);
            }
        }
    }
}
