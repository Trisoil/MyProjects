using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Language
{
    /// <summary>
    /// 基础语言实体
    /// </summary>
    [Serializable]
    public class LanguagePack<T> where T: LanguageNodeBase 
    {
        /// <summary>
        /// 语言包
        /// </summary>
        public Dictionary<string, T> Language { get; set; }
    }
}
