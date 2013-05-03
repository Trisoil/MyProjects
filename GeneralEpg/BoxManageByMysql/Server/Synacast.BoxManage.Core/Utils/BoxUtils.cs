using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core.Utils
{
    using log4net;

    /// <summary>
    /// 核心帮助类
    /// </summary>
    public class BoxUtils
    {
        public static ILog GetLogger(string name)
        {
            log4net.Config.XmlConfigurator.Configure();
            return LogManager.GetLogger(name);
        }

        public static object GetXmlCData(string value)
        {
            if (string.IsNullOrEmpty(value))
                return string.Empty;
            else
                return new XCData(value);    //string.Format("<![CDATA[{0}]]>", value);//
        }

        /// <summary>
        /// 格式化错误输出
        /// </summary>
        /// <param name="ex"></param>
        /// <returns></returns>
        public static string FormatErrorMsg(Exception ex)
        {
            var xml = new XElement("err", new XCData(ex.Message));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        public static XElement FormatXmlErrorMsg(Exception ex)
        {
            return new XElement("err", new XCData(ex.Message));
        }

        public static string FormatErrorMsg(string error)
        {
            var xml = new XElement("err", new XCData(error));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 判断输入字符是否为汉字
        /// </summary>
        /// <param name="word">输入字符</param>
        /// <returns></returns>
        public static bool IsChinese(string word)
        {
            return System.Text.RegularExpressions.Regex.IsMatch(word, @"[\u4e00-\u9fa5]+$");
        }

        //public static string GetAppSetting(string elementName, string attributeName)
        //{
        //    string path = string.Format("{0}/Config/BoxAppSetting.xml", AppDomain.CurrentDomain.BaseDirectory);
        //    XElement root = XElement.Load(path);
        //    return (string)root.Element(elementName).Attribute(attributeName);
        //}

        //public static int GetAppSetting(string elementName, string attributeName, int defaultValue)
        //{
        //    string path = string.Format("{0}/Config/BoxAppSetting.xml", AppDomain.CurrentDomain.BaseDirectory);
        //    XElement root = XElement.Load(path);
        //    int value;
        //    if (!int.TryParse((string)root.Element(elementName).Attribute(attributeName), out value))
        //        value = defaultValue;
        //    return value;
        //}
    }

}
