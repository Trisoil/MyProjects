using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Storage.Streams;
using Windows.ApplicationModel.DataTransfer;

namespace PPTVForWin8.Utils
{
    using PPTV.WinRT.CommonLibrary.DataModel.Play;

    public static class ShareUtils
    {
        public static PlayShareInfo ShareInfo;

        public static void Share(DataRequest request)
        {
            if (ShareInfo != null)
            {
                request.Data.Properties.Title = ShareInfo.Title;
                request.Data.Properties.Description = "PPTV 网络电视";
                request.Data.SetText(
                    string.Format(Constants.ShareTips, ShareInfo.Title, PPTVData.Utils.DataCommonUtils.ConvertToWebSite(ShareInfo.Id)));
                var reference = RandomAccessStreamReference.CreateFromUri(new Uri(ShareInfo.ImageUri));
                request.Data.Properties.Thumbnail = reference;
                var deferral = request.GetDeferral();
                request.Data.SetBitmap(reference);
                deferral.Complete();
            }
        }
    }
}
