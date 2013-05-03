using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

using PPTVData.Entity;
using PPTVData.Factory;
using System.Threading;
namespace PPTVForWP7.Controls
{
    public partial class HotSearchBar : UserControl
    {
        public delegate void ImageTapEventHandler(object sender, HotSearchInfo hotSearchInfo);
        public event ImageTapEventHandler ItemTap;
        public HotSearchBar()
        {
            InitializeComponent();
            _hotSearchFactory = new HotSearchFactory();
            _hotSearchFactory.HttpFailHandler += _hotSearchFactory_httpFailHandler;
            _hotSearchFactory.HttpSucessHandler += _hotSearchFactory_httpSucessHandler;
            _hotSearchFactory.HttpTimeOutHandler += _hotSearchFactory_httpTimeOutHandler;
            Init();
        }
        double _lChar = 13.3;
        double _cChar = 15;
        double _number = 13.3;
        double _point = 6.3;
        double _chine = 23;
        int _firstMaxChangedLeftMargin = 40;
        int _firstMinChangedLeftMargin = 5;
        double _margin = 25.0;
        double _totalWidth = 433.0;
        HotSearchFactory _hotSearchFactory;
        List<HotSearchInfo> _hotSearchInfos;
        List<HotSearchInfo> _hotSearchInfosByDisplay;
        public void Init()
        {
            _hotSearchFactory.DownLoadDatas(null);
        }

        void _hotSearchFactory_httpFailHandler(object sender, HttpFactoryArgs<List<HotSearchInfo>> args)
        {
        }

        void _hotSearchFactory_httpSucessHandler(object sender, HttpFactoryArgs<List<HotSearchInfo>> args)
        {
            if (args.Result == null 
                || args.Result.Count <= 0)
                return;
            _hotSearchInfos = new List<HotSearchInfo>();
            _hotSearchInfosByDisplay = new List<HotSearchInfo>();
            for (int i = 0; i < (args.Result.Count >= 6 ? 6 : args.Result.Count); i++)
            {
                HotSearchInfo itemCopy = new HotSearchInfo(args.Result[i].Type, args.Result[i].Count.ToString(), args.Result[i].Key);
                _hotSearchInfos.Add(args.Result[i]);
                _hotSearchInfosByDisplay.Add(itemCopy);
            }
            SortByCharLenght();
            InitElement();
        }

        private void SortByCharLenght()
        {
            HotSearchInfo tem;
            for (int i = 0; i < 2; i++)
            {
                for (int j = i + 1; j < 3; j++)
                {
                    if (_hotSearchInfos.Count >= j
                        && _hotSearchInfos[j].Key.ToCharArray().Length < _hotSearchInfos[i].Key.ToCharArray().Length)
                    {
                        tem = _hotSearchInfos[i];
                        _hotSearchInfosByDisplay[i] = _hotSearchInfos[j];
                        _hotSearchInfos[i] = _hotSearchInfos[j];
                        _hotSearchInfosByDisplay[j] = tem;
                        _hotSearchInfos[j] = tem;
                    }
                }
            }

            for (int i = 3; i < 5; i++)
            {
                for (int j = i + 1; j < 6; j++)
                {
                    if (_hotSearchInfos.Count >= j 
                        && _hotSearchInfos[j].Key.ToCharArray().Length < _hotSearchInfos[i].Key.ToCharArray().Length)
                    {
                        tem = _hotSearchInfos[i];
                        _hotSearchInfosByDisplay[i] = _hotSearchInfos[j];
                        _hotSearchInfos[i] = _hotSearchInfos[j];
                        _hotSearchInfosByDisplay[j] = tem;
                        _hotSearchInfos[j] = tem;
                    }
                }
            }
        }

        private void InitElement()
        {
            int left1 = new Random().Next(_firstMaxChangedLeftMargin - _firstMinChangedLeftMargin) + _firstMinChangedLeftMargin;

            double totalLeft1ForCalc = 0.0;
            double totalLeft2ForCalc = 0.0;
            xHotSearchItemsGrid.Children.Clear();
            Thread.Sleep(TimeSpan.FromMilliseconds(50));
            int left2 = new Random().Next(_firstMaxChangedLeftMargin - _firstMinChangedLeftMargin) + _firstMinChangedLeftMargin;
            //Calc
            for (int j = 0; j < _hotSearchInfosByDisplay.Count; j++)
            {
                if (j <= 2)
                {
                    if (j == 0)
                    {
                        totalLeft1ForCalc += left1 + GetStringWidth(_hotSearchInfosByDisplay[j].Key) + _margin;
                    }
                    else
                    {
                        totalLeft1ForCalc += _margin + GetStringWidth(_hotSearchInfosByDisplay[j].Key);
                        if (j == 2)
                        {
                            if (totalLeft1ForCalc > _totalWidth)
                            {
                                int spliteCount = Convert.ToInt32((totalLeft1ForCalc - _totalWidth + _point * 3) / _chine);
                                int totalLength = _hotSearchInfosByDisplay[2].Key.ToArray().Length;
                                try
                                {
                                    _hotSearchInfosByDisplay[2].Key = _hotSearchInfosByDisplay[2].Key.Substring(0, ((totalLength - spliteCount > totalLength || totalLength - spliteCount < 0) ? totalLength : totalLength - spliteCount)) + "...";
                                }
                                catch
                                {
                                    _hotSearchInfosByDisplay[2].Key = "...";
                                }
                            }
                        }
                    }
                }
                else if (j > 2)
                {
                    if (j == 3)
                    {
                        totalLeft2ForCalc += left2 + GetStringWidth(_hotSearchInfosByDisplay[j].Key) + _margin;
                    }
                    else
                    {
                        totalLeft2ForCalc += _margin + GetStringWidth(_hotSearchInfosByDisplay[j].Key);

                        if (j == 5)
                        {
                            if (totalLeft2ForCalc > _totalWidth)
                            {
                                int spliteCount = Convert.ToInt32((totalLeft2ForCalc - _totalWidth + _point * 3) / _chine);
                                int totalLength = _hotSearchInfosByDisplay[5].Key.ToArray().Length;
                                try
                                {
                                    _hotSearchInfosByDisplay[5].Key = _hotSearchInfosByDisplay[5].Key.Substring(0, ((totalLength - spliteCount > totalLength || totalLength - spliteCount < 0) ? totalLength : totalLength - spliteCount)) + "...";
                                }
                                catch
                                {
                                    _hotSearchInfosByDisplay[5].Key = "...";
                                }
                            }
                        }
                    }
                }
            }
            double totalLeft1 = 0.0;
            double totalLeft2 = 0.0;
            //
            for (int i = 0; i < _hotSearchInfosByDisplay.Count; i++)
            {
                Button button = new Button();
                button.Foreground = new SolidColorBrush(Colors.White);
                button.Content = _hotSearchInfosByDisplay[i].Key;
                button.HorizontalAlignment = HorizontalAlignment.Left;
                button.VerticalAlignment = VerticalAlignment.Top;
                button.Style = Resources["HotSearchButtonStyle"] as Style;
                button.Tag = _hotSearchInfos[i];
                button.Tap += new EventHandler<GestureEventArgs>(button_Tap);
                if (i <= 2)
                {
                    if (i == 0)
                    {
                        button.Margin = new Thickness(0, 10, 0, 0);
                        totalLeft1 += GetStringWidth(_hotSearchInfosByDisplay[i].Key) + _margin;
                    }
                    else
                    {
                        button.Margin = new Thickness(totalLeft1, 10, 0, 0);
                        totalLeft1 += _margin + GetStringWidth(_hotSearchInfosByDisplay[i].Key);
                    }
                    xHotSearchItemsGrid.Children.Add(button);
                }
                else if (i > 2)
                {
                    if (i == 3)
                    {
                        button.Margin = new Thickness(0, 60, 0, 0);
                        totalLeft2 += GetStringWidth(_hotSearchInfosByDisplay[i].Key) + _margin;
                    }
                    else
                    {
                        button.Margin = new Thickness(totalLeft2, 60, 0, 0);
                        totalLeft2 += _margin + GetStringWidth(_hotSearchInfosByDisplay[i].Key);
                    }
                    xHotSearchItemsGrid.Children.Add(button);
                }
            }
        }

        void button_Tap(object sender, GestureEventArgs e)
        {
            if (ItemTap != null)
                ItemTap(sender, (sender as Button).Tag as HotSearchInfo);
        }

        private double GetStringWidth(string str)
        {
            char[] chars = str.ToCharArray();
            double width = 0;
            for (int i = 0; i < chars.Length; i++)
            {
                int ascal = chars[i];
                if (ascal == 46)
                {
                    width += _point;
                }
                else if (ascal >= 48 && ascal <= 57)
                {
                    width += _number;
                }
                else if (ascal >= 65 && ascal <= 90)
                {
                    width += _cChar;
                }
                else if (ascal >= 98 && ascal <= 122)
                {
                    width += _lChar;
                }
                else
                {
                    //if (chars.Length > 2)
                    width += _chine;
                }
            }
            return width;
        }
        void _hotSearchFactory_httpTimeOutHandler(object sender, HttpFactoryArgs<List<HotSearchInfo>> args)
        {

        }

        private void BeginGetData()
        {

        }
    }
}
