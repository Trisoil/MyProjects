using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel.Play
{
    using PPTVData.Entity;

    public class PlayMessageInfo
    {
        public bool HasPreProgram { get; set; }

        public bool HasNextProgram { get; set; }

        public int StartPosition { get; set; }

        public string Title { get; set; }

        public Dictionary<int, PlayInfo> PlayInfos { get; set; }

        public PlayInfo CurrentPlayInfo { get; set; }

        public PlayShareInfo ShareInfo { get; set; }
    }
}
