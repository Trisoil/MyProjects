using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Navigation;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Play
{
    using PPTVData.Entity;

    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.DataModel.Download;

    public class PlayAdapter
    {
        public static IPlayFactory CreatePlayFactory(NavigationContext navigationContext)
        {
            var playType = (PlayType)Convert.ToInt32(navigationContext.QueryString["playtype"]);

            switch (playType)
            { 
                case PlayType.Download:
                    return new PlayDownloadFactory(Convert.ToInt32(navigationContext.QueryString["id"]));
                default:
                    var id = Convert.ToInt32(navigationContext.QueryString["id"]);
                    var index = Convert.ToInt32(navigationContext.QueryString["index"]);
                    return new PlayVodFactory(new PlayInfoHelp(id, index));
            }

        }
    }
}
