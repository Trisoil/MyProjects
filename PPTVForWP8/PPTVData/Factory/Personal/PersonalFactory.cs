using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public class PersonalFactory : FileFactoryBase<PersonalFactory, PersonalInfo>
    {
        public Action<bool> LoginChangeAction;

        public override string FileName
        {
            get { return "PersonalInfo.db"; }
        }

        /// <summary>
        /// 是否包含登录信息
        /// </summary>
        public bool HaveLoginInfo
        {
            get
            {
                return DataInfos.Count > 0
                    && DataInfos[0].IsSaveInfo;
            }
        }

        /// <summary>
        /// 是否已经登录
        /// </summary>
        public bool Logined
        {
            get
            {
                return DataInfos.Count > 0
                 && DataInfos[0].IsCurrentLogin;
            }
        }

        public void LoginOut()
        {
            if (DataInfos.Count > 0)
            {
                DataInfos[0].IsCurrentLogin = false;
                if (LoginChangeAction != null)
                    LoginChangeAction(false);
            }
        }

        public override async Task Init()
        {
            await base.Init();
            if (HaveLoginInfo)
            {
                if (DataInfos[0].IsAutoLogin)
                    DataInfos[0].IsCurrentLogin = true;
                else
                    DataInfos[0].IsCurrentLogin = false;
            }
        }

        public override void InsertRecord(PersonalInfo record)
        {
            DataInfos.PerformanceClear();
            base.InsertRecord(record);
            if (LoginChangeAction != null)
                LoginChangeAction(true);
        }
    }
}
