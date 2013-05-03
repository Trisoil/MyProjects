using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Sync
{
    /// <summary>
    /// 代表网络中的一个节点
    /// </summary>
    public class NetNode
    {
        #region Fields

        private List<NetInfo> _children;

        #endregion

        /// <summary>
        /// 该节点的详细信息
        /// </summary>
        public NetInfo Info { get; set; }

        /// <summary>
        /// 该节点的直接父节点
        /// </summary>
        public NetInfo Parent { get; set; }

        /// <summary>
        /// 该节点的所有孩子节点
        /// </summary>
        public List<NetInfo> Children
        {
            get { return _children; }
            set { _children = value; }
        }

        #region Methods

        /// <summary>
        /// 向该节点添加孩子节点
        /// </summary>
        /// <param name="child"></param>
        public void AddChild(NetInfo child)
        {
            if (_children == null)
                _children = new List<NetInfo>();
            _children.Add(child);
        }

        #endregion
    }
}
