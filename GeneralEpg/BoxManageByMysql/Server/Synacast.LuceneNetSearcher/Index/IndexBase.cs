using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Index
{
    using PanGu;
    using Lucene.Net.Index;
    using Lucene.Net.Store;
    using Lucene.Net.Analysis;
    using Lucene.Net.Documents;
    using Synacast.LuceneNetSearcher.Analyzer;
    using Synacast.LuceneNetSearcher.Cache;

    public class IndexBase
    {
        private Directory _dir;
        private IndexWriter _write;
        private IndexWriter _updatewrite;

        public IndexBase()
            : this(new PanGuAnalyzer(), new RAMDirectory())
        { }

        public IndexBase(Directory directory)
            : this(new PanGuAnalyzer(), directory)
        { }

        public IndexBase(Analyzer analyzer, Directory directory)
        {
            _dir = directory;
            _write = new IndexWriter(directory, analyzer, true, IndexWriter.MaxFieldLength.UNLIMITED);
        }

        public Directory Directory
        {
            get {
                return _dir;
            }
        }

        public IndexWriter Writer
        {
            get {
                return _write;
            }
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public virtual void AddIndex(IndexNode node)
        {
            _write.AddDocument(CreateDoucment(node));
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public virtual void AddIndex(List<IndexNode> nodes)
        {
            foreach (var node in nodes)
            {
                AddIndex(node);
            }
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public virtual void AddIndex(Document doc)
        {
            _write.AddDocument(doc);
        }

        /// <summary>
        /// 添加索引
        /// </summary>
        public virtual void AddIndex(List<Document> docs)
        {
            foreach (var doc in docs)
            {
                AddIndex(doc);
            }
        }

        /// <summary>
        /// 完成索引的添加，关闭Write
        /// </summary>
        public virtual void CompleteIndex()
        {
            _write.Optimize();
            _write.Close();
        }

        public void InitUpdate()
        {
            _updatewrite = new IndexWriter(_dir, new PanGuAnalyzer(), false, IndexWriter.MaxFieldLength.UNLIMITED);
        }

        public void InsertIndex(List<IndexNode> nodes)
        {
            var write = new IndexWriter(_dir, new PanGuAnalyzer(), false, IndexWriter.MaxFieldLength.UNLIMITED);
            foreach (var node in nodes)
            {
                write.AddDocument(CreateDoucment(node));   
            }
            write.Optimize();
            write.Close();
        }

        /// <summary>
        /// 更新索引
        /// </summary>
        public void UpdateDocument(string field, string value, IndexNode node)
        {
            _updatewrite.UpdateDocument(new Term(field, value), CreateDoucment(node));
        }

        public void CompleteUpdate()
        {
            _updatewrite.Optimize();
            _updatewrite.Close();
        }

        public void DeleteDocument(Term[] terms)
        {
            var write = new IndexWriter(_dir, new PanGuAnalyzer(), false, IndexWriter.MaxFieldLength.UNLIMITED);
            write.DeleteDocuments(terms);
            write.Optimize();
            write.Close();
        }

        /// <summary>
        /// 创建Document
        /// </summary>
        private Document CreateDoucment(IndexNode node)
        {
            Document doc = new Document();
            foreach (KeyValuePair<string, string> name in node.IndexNames)
            {
                var field = new Field(name.Key, name.Value, Field.Store.YES, Field.Index.ANALYZED);
                var boost = SettingCache.Boost;
                switch (name.Key)
                {
                    case "name":
                        break;
                    case "catalog":
                        boost = boost - 4; break;
                    case "act":
                        boost = boost - 8; break;
                    case "area":
                        boost = boost - 16; break;
                }
                field.SetBoost(boost);
                doc.Add(field);
            }
            foreach (KeyValuePair<string, string> value in node.IndexValues)
            {
                doc.Add(new Field(value.Key, value.Value, Field.Store.YES, Field.Index.NOT_ANALYZED));
            }
            foreach (KeyValuePair<string, int> numeric in node.IndexNumerices)
            {
                doc.Add(new NumericField(numeric.Key, Field.Store.YES, true).SetIntValue(numeric.Value));
            }
            if (node.IndexLongs != null)
            {
                foreach (var numeric in node.IndexLongs)
                {
                    doc.Add(new NumericField(numeric.Key, Field.Store.YES, true).SetLongValue(numeric.Value));
                }
            }
            return doc;
        }
    }
}
