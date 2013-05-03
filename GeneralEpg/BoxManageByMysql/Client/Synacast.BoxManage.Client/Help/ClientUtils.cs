using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Xml.Linq;
using Microsoft.VisualBasic;

namespace Synacast.BoxManage.Client.Help
{
    using log4net;

    using Synacast.BoxManage.Client.Handler;

    public class ClientUtils
    {
        static char[] _splitdh = new char[] { ',' };

        public static ILog GetLogger(string name)
        {
            log4net.Config.XmlConfigurator.Configure();
            return LogManager.GetLogger(name);
        }

        public static string FormatErrorMsg(string error)
        {
            var xml = new XElement("err", GetXmlCData(error));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        public static object GetXmlCData(string value)
        {
            if (string.IsNullOrEmpty(value))
                return string.Empty;
            else
                return new XCData(value);
        }

        public static int FormatType(string type)
        {
            int ftype = 0;
            int.TryParse(type, out ftype);
            if (ftype <= 0 || ftype > 7)
                ftype = 1;
            return ftype;
        }

        public static int FormatListType(string type, string source)
        {
            if (string.IsNullOrEmpty(source))
                return FormatInt(type);
            return FormatType(type);
        }

        public static int FormatListType(string type, string source1, string source2, int source3)
        {
            if (string.IsNullOrEmpty(source1) && string.IsNullOrEmpty(source2) && source3 == 0)
                return FormatInt(type);
            return FormatType(type);
        }

        public static double FormatDouble(string source, string name)
        {
            double result;
            if (!double.TryParse(source, out result) || result <= 0)
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            return result;
        }

        public static int FormatInt(string source)
        {
            int fsource = 0;
            if (!int.TryParse(source, out fsource) || fsource <= 0)
                return 0;
            return fsource;
        }

        public static int FormatIntMax(string source)
        {
            int fsource = 0;
            if (!int.TryParse(source, out fsource) || fsource <= 0)
                return int.MaxValue;
            return fsource;
        }

        public static int FormatInt(string source, int maxValue)
        {
            int fsource = 0;
            int.TryParse(source, out fsource);
            if (fsource > maxValue)
                return maxValue;
            return fsource;
        }

        public static int FormatDefaultInt(string source, int defaultValue)
        {
            if (!string.IsNullOrEmpty(source) && source.Contains(","))
                source = source.Split(_splitdh)[0];
            int fsource = 0;
            if (int.TryParse(source, out fsource))
                return fsource;
            return defaultValue;
        }

        public static int FormatIntV(string source, string name)
        {
            int fsource = 0;
            if (!int.TryParse(source, out fsource) || fsource <= 0)
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            return fsource;
        }

        public static int FormatIntV(string source, int maxValue, string name)
        {
            int fsource = 0;
            if (!int.TryParse(source, out fsource) || fsource <= 0)
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            if (fsource > maxValue)
                return maxValue;
            return fsource;
        }

        public static bool FormatBool(string source)
        {
            if (string.IsNullOrEmpty(source))
                return false;
            var fsource = 0;
            int.TryParse(source, out fsource);
            if (fsource <= 0)
                return false;
            return true;
        }

        public static string FormatOrder(string source)
        {
            if (string.IsNullOrEmpty(source))
                return "s";
            return source;
        }

        public static string FormatStrV(string source, string name)
        {
            if (string.IsNullOrEmpty(source))
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            return source;
        }

        public static DateTime FormatDateTime(string source, DateTime defaultValue)
        {
            DateTime fvalue;
            DateTime.TryParse(source.Replace("--", "  "), out fvalue);
            if (fvalue == null || fvalue == DateTime.MinValue)
                return defaultValue;
            return fvalue;
        }

        public static DateTime FormatDateTime(string source)
        {
            DateTime fvalue;
            string[] sources = source.Split(new string[] { "-" }, StringSplitOptions.RemoveEmptyEntries);
            if (sources.Length != 6)
                ResponseError("输入的日期格式不正确，应为yyyy-MM-dd--HH-mm-ss");
            DateTime.TryParse(string.Format("{0}-{1}-{2}  {3}:{4}:{5}", sources[0], sources[1], sources[2], sources[3], sources[4], sources[5]), out fvalue);
            if (fvalue == null || fvalue == DateTime.MinValue)
                ResponseError("输入的日期格式不正确，应为yyyy-MM-dd--HH-mm-ss");
            if (fvalue.CompareTo(DateTime.Now.Date) < 0)
                ResponseError("时间跨度过大，应为当天时间，请重新获得全部数据");
            return fvalue;
        }

        public static DateTime FormatDateTime(string source, string name)
        {
            return FormatDateTime(FormatStrV(source, name));
        }

        /// <summary>
        /// 格式化直播日期输入
        /// </summary>
        public static DateTime FormatLiveDate(string source)
        {
            DateTime result;
            if (!DateTime.TryParse(source, out result))
                ResponseError("输入的日期格式不正确，应为yyyy-MM-dd");
            return result;
        }

        /// <summary>
        /// 直播节目预告搜索日期格式化
        /// </summary>
        public static long FormatLiveParadeSearchDate(string source)
        {
            if (!string.IsNullOrEmpty(source))
            {
                DateTime result;
                if (!DateTime.TryParse(source, out result))
                    ResponseError("输入的日期格式不正确，应为yyyy-MM-dd");
                return result.Date.AddDays(1).AddSeconds(-1).Ticks;
            }
            return 0;
        }

        public static string FormatKey(string key)
        {
            if (string.IsNullOrEmpty(key))
                ResponseError("关键字不能为空");
            return key;
        }

        public static string FormatRankingSpan(string source)
        {
            if (source != "day" || source != "week" || source != "quarter" || source != "year")
                return "month";
            return source;
        }

        public static string FormatBeginLetter(string source, string name)
        {
            if (!string.IsNullOrEmpty(source))
            {
                bool result = char.IsLetter(source, 0);
                if (!result)
                    ResponseError(string.Format("输入的参数{0}非法", name));
                else
                    return source[0].ToString().ToLower();
            }
            return source;
        }

        public static string FormatLanguage(string source)
        {
            if (string.IsNullOrEmpty(source))
                return "zh_cn";
            return source;
        }

        public static string FormatSimpleChinese(string source)
        {
            var result = Strings.StrConv(source, VbStrConv.SimplifiedChinese, 0);
            if (result.Contains('?'))
                return source;
            return result;
        }

        public static string FormatSimpleChinese(string source, string name)
        {
            if (string.IsNullOrEmpty(source))
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            var result = Strings.StrConv(source, VbStrConv.SimplifiedChinese, 0);
            if (result.Contains('?'))
                return source;
            return result;
        }

        public static string FormatNTags(string source, string name)
        {
            if (string.IsNullOrEmpty(source) || !source.Contains(":"))
                ResponseError(string.Format("输入的参数{0}非法或为空", name));
            var result = Strings.StrConv(source, VbStrConv.SimplifiedChinese, 0);
            if (result.Contains('?'))
                return source;
            return result;
        }

        public static string FormatString(string source, string name)
        {
            if (string.IsNullOrEmpty(source))
                ResponseError(string.Format("输入的参数{0}不能为空", name));
            return source;
        }

        public static void ResponseError(string errmsg)
        {
            HttpContext.Current.Server.Transfer(new ErrorHandler(errmsg), false);
        }
    }
}
