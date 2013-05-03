using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Factory.Play
{
    using PPTVData.Entity;

    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.DataModel.Download;

    public class PlayAdapter
    {
        public static IPlayFactory CreatePlayFactory(object parameter)
        {
            IPlayFactory playFactory = null;

            if (parameter is LiveListItem)
            {
                playFactory = new PlayLiveFactory(parameter);
            }
            else if (parameter is DownloadInfo)
            {
                playFactory = new PlayDownloadFactory(parameter);
            }
            else
            {
                playFactory = new PlayVodFactory(parameter);
            }

            return playFactory;
        }
    }
}
