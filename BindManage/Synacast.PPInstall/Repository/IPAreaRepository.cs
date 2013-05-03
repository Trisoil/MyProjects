using System;
using System.Collections.Generic;
using System.Text;

namespace Synacast.PPInstall.Repository
{
    using Synacast.PPInstall;
    using Synacast.PPInstall.Cache;

    public class IPAreaRepository
    {
        public IPAreaNode[] GetIPAreaTree()
        {
            List<IPAreaNode> tree = new List<IPAreaNode>();
            //_iplist.AddRange(IPAreaTableInfo.FindAll());
            IPAreaCache.Instance.Items.ForEach(delegate(IPAreaTableInfo info) {
                IPAreaNode parent = tree.Find(delegate(IPAreaNode node) { return node.attributes.Country == info.Country; });
                if (parent != null)
                {
                    List<IPAreaNode> provinces = new List<IPAreaNode>();
                    if (parent.Children != null)
                    {
                        provinces.AddRange(parent.Children);
                    }
                    IPAreaNode provinceparent = provinces.Find(delegate(IPAreaNode node) { return node.attributes.Province == info.Province; });
                    if (provinceparent != null && !string.IsNullOrEmpty(info.City))
                    {
                        IPAreaNode citynode = new IPAreaNode();
                        citynode.text = info.City;
                        citynode.AreaText = string.Format("{0}/{1}/{2}", info.Country, info.Province, info.City);
                        citynode.attributes = info;
                        provinceparent.AddChildren(citynode);
                    }
                    else
                        LoadChildren(parent, info);
                }
                else
                {
                    if (!string.IsNullOrEmpty(info.Country))
                    {
                        IPAreaNode countrynode = new IPAreaNode();
                        countrynode.text = info.Country;
                        countrynode.AreaText = info.Country;
                        countrynode.attributes = info;
                        tree.Add(countrynode);
                        LoadChildren(countrynode, info);
                    }
                }
            });
            return tree.ToArray();
        }

        private void LoadChildren(IPAreaNode parent,IPAreaTableInfo info)
        {
            if (!string.IsNullOrEmpty(info.Province))
            {
                IPAreaNode newnode = new IPAreaNode();
                newnode.text = info.Province;
                newnode.AreaText = string.Format("{0}/{1}", info.Country, info.Province);
                newnode.attributes = info;
                parent.AddChildren(newnode);
                if (!string.IsNullOrEmpty(info.City))
                {
                    IPAreaNode citynode = new IPAreaNode();
                    citynode.text = info.City;
                    citynode.AreaText = string.Format("{0}/{1}/{2}", info.Country, info.Province, info.City);
                    citynode.attributes = info;
                    newnode.AddChildren(citynode);
                }
            }
        }
    }

    public class IPAreaNode
    {
        private string _text;

        public string text
        {
            get { return _text; }
            set { _text = value; }
        }

        private string _areaText;

        public string AreaText
        {
            get { return _areaText; }
            set { _areaText = value; }
        }

        private IPAreaTableInfo _attributes;

        public IPAreaTableInfo attributes
        {
            get { return _attributes; }
            set { _attributes = value; }
        }

        private List<IPAreaNode> _children;

        public IPAreaNode[] Children
        {
            get { return _children == null ? null : _children.ToArray(); }
        }

        public void AddChildren(IPAreaNode node)
        {
            if (_children == null)
                _children = new List<IPAreaNode>();
            _children.Add(node);
        }

        public void AddChildren(IPAreaTableInfo info, string text)
        {
            if (_children == null)
                _children = new List<IPAreaNode>();
            IPAreaNode node = new IPAreaNode();
            node.text = text;
            node.attributes = info;
            _children.Add(node);
        }
    }
}
