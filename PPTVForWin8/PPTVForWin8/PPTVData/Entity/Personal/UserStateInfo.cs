using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public class UserStateInfo
    {
        /// <summary>
        /// 用户名
        /// </summary>
        public string UserName { get; set; }

        /// <summary>
        /// VIP标识
        /// </summary>
        public int VIP { get; set; }

        /// <summary>
        /// 头像地址
        /// </summary>
        public string FaceUrl { get; set; }

        /// <summary>
        /// 等级
        /// </summary>
        public int Degree { get; set; }

        /// <summary>
        /// 积分
        /// </summary>
        public double Mark { get; set; }

        /// <summary>
        /// 经验值
        /// </summary>
        public double Expence { get; set; }

        /// <summary>
        /// 用户状态
        /// </summary>
        public int UserState { get; set; }

        /// <summary>
        /// 时间戳
        /// </summary>
        public long Date { get; set; }

        /// <summary>
        /// 过期时间
        /// </summary>
        public long OutDate { get; set; }

        /// <summary>
        /// 会员过期时间
        /// </summary>
        public long VIPDate { get; set; }

        /// <summary>
        /// VIP会员类型
        /// </summary>
        public int VIPType { get; set; }

        public string MD5 { get; set; }

        public string Token { get; set; }

        public string P13 { get; set; }

        public string P14 { get; set; }

        public string P15 { get; set; }
    }
}
