using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Synacast.BoxManage.Tests
{
    //using Synacast.BoxManage.Core;
    //using Synacast.BoxManage.Core.Help;
    //using Synacast.BoxManage.Core.Cache.List;
    //using Synacast.BoxManage.Core.Job;

    //using Synacast.ServicesFramework;
    //using Synacast.BoxManage.Core.Cache;
    //using Synacast.BoxManage.Core.Sync;

    /// <summary>
    /// UnitTest1 的摘要说明
    /// </summary>
    [TestClass]
    public class ListCacheTest
    {
        public ListCacheTest()
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
            //init.Init();
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
        public void TestFormatBoxType()
        {
            //Assert.AreEqual(1, BoxUtils.FormatBoxType("71781"));
            //List<string> result = new List<string>();
            //BoxUtils.ExecFindCatalog(72389,result);
            //Assert.AreEqual<bool>(true, result.Count > 0);
            //Assert.AreEqual<string>("电影", result[0]);
            //BoxTypeCache.Instance.SerializeInit();
            //ListCache.Instance.SerializeInit();
            //TagCache.Instance.SerializeInit();
            //NetNodeService.Instance.Init();
            //ListCacheJob job = new ListCacheJob();
            //NetNodeService.Instance.SynaChildrenData(ListCache.Instance.Items, TagCache.Instance.Items, BoxTypeCache.Instance.Items);
        }

        //[TestMethod,DeploymentItem("Config","Config")]
        //public void TestUpdateJob()
        //{
        //    UpdateListJob job = new UpdateListJob();
        //    job.Execute(null);
        //}

        //[TestMethod, DeploymentItem("Config", "Config")]
        //public void TestNewListCache()
        //{
        //    Engine engine = new Engine();
        //    engine.Start();
        //    BoxTypeCache.Instance.Init(null);
        //    ListCache.Instance.Init(null);
        //}

        //[TestMethod, DeploymentItem("Config", "Config")]
        //public void TestNewTagCache()
        //{
        //    Engine engine = new Engine();
        //    engine.Start();
        //    ListCache.Instance.SerializeInit();
        //    TagCache.Instance.Init(null);
        //}
    }
}
