using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall.Repository
{
    using Synacast.PPInstall;
    using Synacast.PPInstall.Error;
    using Synacast.NHibernateActiveRecord;

    public class GuideRepository
    {
        /// <summary>
        /// 查找是否有相同定制的引导包信息
        /// </summary>
        /// <param name="customId"></param>
        public void JudgeCustom(int customId)
        {
            string sql = string.Format("select count(*) from {0} where CustomID={1}", GuideInfo.TableName, customId);
            int count = (int)GuideInfo.ExecuteScalarBySql(sql);
            if (count > 0)
                throw new ApplicationException(Error.GuideCustomError);
        }
    }
}
