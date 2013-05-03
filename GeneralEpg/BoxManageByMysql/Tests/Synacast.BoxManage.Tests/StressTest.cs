using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Synacast.BoxManage.Tests
{
    using PanGu;
    using NHibernate;
    using Synacast.BoxManage.Core;
    using Synacast.ServicesFramework;
    using Synacast.ServicesFramework.Extension;
    using Synacast.ServicesFramework.NHibernate;
    using Synacast.LuceneNetSearcher;
    using Synacast.LuceneNetSearcher.Utils;
    using Synacast.LuceneNetSearcher.Analyzer;
    using Synacast.BoxManage.Core.Cache.Internal;

    /// <summary>
    /// StressTest 的摘要说明
    /// </summary>
    [TestClass]
    public class StressTest
    {
        public StressTest()
        {
            //
            //TODO: 在此处添加构造函数逻辑
            //
        }

        private TestContext testContextInstance;

        /// <summary>
        ///获取或设置测试上下文，该上下文提供
        ///有关当前测试运行及其功能的信息。
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        [ClassInitialize]
        public static void Init(TestContext testContext)
        {
            //SerializeInitialize init = new SerializeInitialize();
            //init.CommunicationInit();
        }

        #region 附加测试属性
        //
        // 编写测试时，还可使用以下附加属性:
        //
        // 在运行类中的第一个测试之前使用 ClassInitialize 运行代码
        // [ClassInitialize()]
        // public static void MyClassInitialize(TestContext testContext) { }
        //
        // 在类中的所有测试都已运行之后使用 ClassCleanup 运行代码
        // [ClassCleanup()]
        // public static void MyClassCleanup() { }
        //
        // 在运行每个测试之前，使用 TestInitialize 来运行代码
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // 在每个测试运行完之后，使用 TestCleanup 来运行代码
        // [TestCleanup()]
        // public void MyTestCleanup() { }
        //
        #endregion

        [TestMethod, DeploymentItem("Config", "Config")]
        public void EngineTest()
        {
            Engine engine = new Engine();
            engine.Start();
        }

        [TestMethod, DeploymentItem("Config", "Config")]
        public void ContainAnalyzerTest()
        {
            LuceneEngine engine = new LuceneEngine();
            engine.Init();
            //string text = "三国演义";
            //string text = "a|f|g|i|m";
            string text = "NBA常规赛-快船vs凯尔特人";
            TextReader tr = new StringReader(text);
            PanGuTokenizer ct = new PanGuTokenizer(tr);
            int end = 0;
            Lucene.Net.Analysis.Token t;
            string ss = string.Empty;
            while (end < text.Length)
            {
                t = ct.Next();
                end = t.EndOffset();
                ss = ss + t.TermText() + "/ ";
            }
            string xxx = ss;
        }

        [TestMethod, DeploymentItem("Config", "Config")]
        public void SpellTest()
        {
            
            LuceneEngine engine = new LuceneEngine();
            engine.Init();
            var firsts = SpellUtils.GetCnSegment("NBA常规赛-快船vs凯尔特人");
            var spells = SpellUtils.GetSpellSegment("战重警和");
            var segment = new Segment();
            var collection = segment.DoSegment("国际足球100509K联赛釜山-大田");
            var list = new List<string>();
            foreach (WordInfo word in collection)
            {
                if (word == null)
                    continue;
                list.Add(word.Word);
            }
            //var list = SpellUtils.GetSpellSegment("战警");
            string sss = "san国yan义";
            string ssss = Synacast.LuceneNetSearcher.Searcher.Searcher.SegmentKeyWord(sss);
            string text = "重庆";
            TextReader tr = new StringReader(text);
            PanGuTokenizer ct = new PanGuTokenizer(tr);
            int end = 0;
            Lucene.Net.Analysis.Token t;
            string ss = string.Empty;
            while (end < text.Length)
            {
                t = ct.Next();
                end = t.EndOffset();
                ss = ss + t.TermText() + "/ ";
            }
        }

        [TestMethod, DeploymentItem("Config", "Config")]
        public void NHibernateTest()
        {
            Engine engine = new Engine();
            engine.Start();
            using (ISession session = SessionManage.OpenSession())
            {
                ITransaction tran = session.BeginTransaction();
                string sql = string.Format("from BoxTagsInfo b");
                IList list = session.CreateQuery(sql).List();
                tran.Commit();
            }
        }

        [TestMethod, DeploymentItem("Config", "Config")]
        public void AuthNodeTest()
        {
            //PlatFormCache.Instance.Init();
            Synacast.BoxManage.Core.LuceneNet.EpgLuceneNet.Instance.Init();
        }
 
    }

    public class BoxTagsInfo
    {
        public virtual int ID { get; set; }

        public virtual string Name { get; set; }

        public virtual int TypeId { get; set; }

        public virtual string BKTypes { get; set; }

        public virtual int Rank { get; set; }
    }

    public class FExtension : IExtension
    {
        #region IExtension 成员

        public object Execute()
        {
            return "f1";
        }

        #endregion
    }

    public class AExtension : IExtension
    {
        #region IExtension 成员

        public object Execute()
        {
            return "a1";
        }

        #endregion
    }

    public class AExtension2 : IExtension
    {
        #region IExtension 成员

        public object Execute()
        {
            return "a2";
        }

        #endregion
    }
}
