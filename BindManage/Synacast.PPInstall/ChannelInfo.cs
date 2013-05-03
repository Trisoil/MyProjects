using System;
using System.Collections.Generic;
using System.Text;

using Castle.ActiveRecord;
using Synacast.NHibernateActiveRecord;

namespace Synacast.PPInstall
{
    [ActiveRecord("Channel")]
    public class ChannelInfo:DbObject<ChannelInfo>
    {
        private int _channelID;

        [PrimaryKey]
        public int ChannelID
        {
            get { return _channelID; }
            set { _channelID = value; }
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

        private string _description;

        [Property(Length=512)]
        public string Description
        {
            get { return _description; }
            set { _description = value; }
        }

        private string _type;

        [Property]
        public string Type
        {
            get { return _type; }
            set { _type = value; }
        }

        public static string TableName
        {
            get { return "Channel"; }
        }

        public static string PrimaryKey
        {
            get { return "ChannelID"; }
        }
    }
}
