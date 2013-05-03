using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.Utils
{
    using Coding4Fun.Toolkit.Controls;

    public static class TileUtils
    {
        public static void CreateBasicToast(string Content, string title = "提示")
        {
            var toast = new ToastPrompt
            {
                Title = title,
                Message = Content,
                TextWrapping = System.Windows.TextWrapping.Wrap,
                Margin = new System.Windows.Thickness(0, 0, 0, 70)
            };
            
            toast.Show();
        }
    }
}
