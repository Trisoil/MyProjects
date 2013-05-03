using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Product")]
    public class ProductInfo:DbObject<ProductInfo> 
    {
        private int _productID;

        [PrimaryKey]
        public int ProductID
        {
            get { return _productID; }
            set { _productID = value; }
        }

        private string _cnName;

        [Property]
        public string CnName
        {
            get { return _cnName; }
            set { _cnName = value; }
        }

        private string _enName;

        [Property]
        public string EnName
        {
            get { return _enName; }
            set { _enName = value; }
        }

        private string _cnDescription;

        [Property(Length=512)]
        public string CnDescription
        {
            get { return _cnDescription; }
            set { _cnDescription = value; }
        }

        private string _enDescription;

        [Property(Length=512)]
        public string EnDescription
        {
            get { return _enDescription; }
            set { _enDescription = value; }
        }

        public static string TableName
        {
            get { return "Product"; }
        }

        public static string PrimaryKey
        {
            get { return "ProductID"; }
        }
    }
}
