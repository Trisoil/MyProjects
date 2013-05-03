using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Cloud
{
    public class LocalFavoritesFactory : LocalBaseFactory<LocalFavoritesFactory>
    {
        public override string FileName
        {
            get { return "filefavorites.db"; }
        }

        public override string CloudTypeName
        {
            get { return CloudType.Favorites; }
        }

        public override int MaxCount
        {
            get
            {
                return 100;
            }
        }
    }
}
