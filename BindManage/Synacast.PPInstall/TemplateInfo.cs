using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Template")]
    public class TemplateInfo:DbObject<TemplateInfo>
    {
        private int _templateID;

        [PrimaryKey]
        public int TemplateID
        {
            get { return _templateID; }
            set { _templateID = value; }
        }

        private int _type;

        [Property]
        public int Type
        {
            get { return _type; }
            set { _type = value; }
        }

        private bool _enableAll;

        [Property]
        public bool EnableAll
        {
            get { return _enableAll; }
            set { _enableAll = value; }
        }

        private int _softCount;

        [Property]
        public int SoftCount
        {
            get { return _softCount; }
            set { _softCount = value; }
        }

        private string _description;

        [Property]
        public string Description
        {
            get { return _description; }
            set { _description = value; }
        }

        private string _name;

        [Property]
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        public static string TableName
        {
            get { return "Template"; }
        }

        public static string PrimaryKey
        {
            get { return "TemplateID"; }
        }
    }
}
