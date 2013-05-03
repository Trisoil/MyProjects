using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public class PersonalInfo
    {
        /// <summary>
        /// 是否保存用户信息
        /// </summary>
        public bool IsSaveInfo { get; set; }

        /// <summary>
        /// 是否自动登录
        /// </summary>
        public bool IsAutoLogin { get; set; }

        /// <summary>
        /// 当前是否登录
        /// </summary>
        public bool IsCurrentLogin { get; set; }

        /// <summary>
        /// 用户密码
        /// </summary>
        public string UserPassWord { get; set; }

        /// <summary>
        /// 用户信息
        /// </summary>
        public UserStateInfo UserStateInfo { get; set; }
    }
}
