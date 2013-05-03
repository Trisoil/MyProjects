using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;

    /// <summary>
    /// 基础语言包处理函数
    /// </summary>
    public static class SimpleLanguageHandler
    {
        public static Dictionary<string, SimpleLanguageNode> ProcessLang(string cnName, params object[] otherNames)
        {
            var langs = new Dictionary<string, SimpleLanguageNode>(AppSettingCache.LanguageNum);
            var cnode = new SimpleLanguageNode() { Title = cnName };
            langs[CustomArray.LanguageArray[0]] = cnode;
            if (AppSettingCache.Instance["LanauageTW"] == "1")
            {
                langs[CustomArray.LanguageArray[1]] = new TraditionSimpleNode(cnode);
            }
            if (AppSettingCache.Instance["LanauageEN"] == "2" && otherNames != null && otherNames.Length > 0)
            {
                langs[CustomArray.LanguageArray[2]] = new SimpleLanguageNode() { Title = Convert.ToString(otherNames[0]) };
            }
            return langs;
        }

        public static Dictionary<string, DoubleLanguageNode> DoubleProcessLang(string title, string subTitle, params object[] otherNames)
        {
            var langs = new Dictionary<string, DoubleLanguageNode>(AppSettingCache.LanguageNum);
            var cnode = new DoubleLanguageNode() { Title = title, SubTitle = subTitle };
            langs[CustomArray.LanguageArray[0]] = cnode;
            if (AppSettingCache.Instance["LanauageTW"] == "1")
            {
                langs[CustomArray.LanguageArray[1]] = new TraditionDoubleNode(cnode);
            }
            if (AppSettingCache.Instance["LanauageEN"] == "2" && otherNames != null && otherNames.Length > 1)
            {
                langs[CustomArray.LanguageArray[2]] = new DoubleLanguageNode() { Title = Convert.ToString(otherNames[0]), SubTitle = Convert.ToString(otherNames[1]) };
            }
            return langs;
        }
    }
}
