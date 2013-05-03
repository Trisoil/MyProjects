using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Controls;

using Microsoft.Phone.Controls;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    public abstract class DataTemplateSelector : ContentControl
    {
        public abstract DataTemplate SelectTemplate(object item, DependencyObject container);
        
        public override void OnApplyTemplate()
        {
            this.ContentTemplate = SelectTemplate(DataContext, this);
            this.SetBinding(ContentProperty, new Binding());

            base.ApplyTemplate();
        }

        protected override void OnContentChanged(object oldContent, object newContent)
        {
            this.ContentTemplate = SelectTemplate(newContent, this);

            base.OnContentChanged(oldContent, newContent);
        }

    }
}
