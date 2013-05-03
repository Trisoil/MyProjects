using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Linq;

namespace PPTVData.Entity.Social
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    public class SocialViewModel : ViewModelBase<SocialViewModel, SocialDataInfo>
    {
        public new Dictionary<SocialType, SocialDataInfo> DataInfos;

        /// <summary>
        /// 加载本地数据库
        /// </summary>
        protected override void LoadFromDatabase()
        {
            DataInfos = new Dictionary<SocialType, SocialDataInfo>(4);
            foreach (var social in DataContext.Table_Social)
            {
                if (social.Type == SocialType.RENREN)
                    RenrenSocialData.JudgeToken(social);
                DataInfos.Add(social.Type, social);
            }
            if (DataInfos.Count <= 0)
            {
                var sina = new SocialDataInfo() { Type = SocialType.SINA, IsBinded = false };
                var tencent = new SocialDataInfo() { Type = SocialType.TENCENT, IsBinded = false };
                var renren = new SocialDataInfo() { Type = SocialType.RENREN, IsBinded = false };
                DataContext.Table_Social.InsertOnSubmit(sina);
                DataContext.Table_Social.InsertOnSubmit(tencent);
                DataContext.Table_Social.InsertOnSubmit(renren);
                DataInfos.Add(SocialType.SINA, sina);
                DataInfos.Add(SocialType.TENCENT, tencent);
                DataInfos.Add(SocialType.RENREN, renren);
                DataContext.SubmitChanges();
            }
        }
    }
}
