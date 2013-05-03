using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Social
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.DataModel.Social;

    public class SocialViewModel : FileFactoryBase<SocialViewModel,SocialDataInfo>
    {
        public override string FileName
        {
            get { return "socialinfos.db"; }
        }

        public override async System.Threading.Tasks.Task Init()
        {
            await base.Init();

            if (DataInfos.Count <= 0)
            {
                var sina = new SocialDataInfo() { Type = SocialType.SINA, IsBinded = false };
                var tencent = new SocialDataInfo() { Type = SocialType.TENCENT, IsBinded = false };
                var renren = new SocialDataInfo() { Type = SocialType.RENREN, IsBinded = false };

                DataInfos.Add(sina);
                DataInfos.Add(tencent);
                DataInfos.Add(renren);

                SubmitChange();
            }
        }

        public SocialDataInfo GetRecord(SocialType socialType)
        {
            return DataInfos.FirstOrDefault(v => v.Type == socialType);
        }
    }
}
