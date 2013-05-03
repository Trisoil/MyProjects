using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.WAY
{
    using PPTVData.Entity.WAY;
    using PPTVData.Utils;

    public abstract class WAYFactoryBase : HttpFactoryBase<WAYGetInfo>
    {
        protected int _index;

        protected static DateTime _getTime;
        protected static long _getTimeMillSeconds;
        protected static string _md5Key = "e5@Ib*cMa0Me9I&87S91%Se5Ya$8O9fU#";

        protected byte[] DESCrypt(string source)
        {
            byte[] bufferOut = null;
            var crytSource = Convert.FromBase64String(source);

            BroccoliProducts.DESCrytography.TripleDES(crytSource,
                ref bufferOut, DataCommonUtils.HexToBytes(KEY[_index]), DataCommonUtils.HexToBytes(KEY[0]), false);

            return bufferOut;
        }

        protected abstract override string CreateUri(params object[] paras);
    }
}
