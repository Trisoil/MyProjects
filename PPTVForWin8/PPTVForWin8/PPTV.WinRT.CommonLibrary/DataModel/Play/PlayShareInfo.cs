using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel.Play
{
    public class PlayShareInfo
    {
        public PlayShareInfo(int id, string title, string imageUri)
        {
            Id = id;
            Title = title;
            ImageUri = imageUri;
        }

        public int Id { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }
    }
}
