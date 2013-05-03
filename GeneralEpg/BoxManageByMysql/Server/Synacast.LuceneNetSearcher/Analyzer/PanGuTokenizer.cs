using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Synacast.LuceneNetSearcher.Analyzer
{
    using PanGu;
    using Lucene.Net.Analysis;
    using Synacast.LuceneNetSearcher.Utils;

    public class PanGuTokenizer : Tokenizer
    {
        #region Fields

        List<WordInfo> _wordList;
        int _position = -1; 
        bool _originalResult = false;
        string _inputText;
        bool _isFlag;
        List<string> _segmentList = new List<string>(10);
        bool _customResult = false;
        static string _indexFlag = "superyp";
        static string _indexCnName = "supercnyp";
        static string[] _splitFlag = new string[] { " ", "," };

        #endregion

        public PanGuTokenizer(TextReader input, bool originalResult)
            : this(input)
        {
            _originalResult = originalResult;
        }

        public PanGuTokenizer(TextReader input)
            : base(input)
        {
            _inputText = base.input.ReadToEnd();

            if (string.IsNullOrEmpty(_inputText))
            {
                char[] readBuf = new char[1024];
                int relCount = base.input.Read(readBuf, 0, readBuf.Length);
                StringBuilder inputStr = new StringBuilder(readBuf.Length);

                while (relCount > 0)
                {
                    inputStr.Append(readBuf, 0, relCount);

                    relCount = input.Read(readBuf, 0, readBuf.Length);
                }

                if (inputStr.Length > 0)
                {
                    _inputText = inputStr.ToString();
                }
            }

            _isFlag = AnalyzInput();
            if (!_isFlag)       //盘古分词
            {
                global::PanGu.Segment segment = new Segment();
                var wordInfos = segment.DoSegment(_inputText);
                foreach (var wi in wordInfos)
                {
                    var list = SpellUtils.GetSpellSegment(wi.Word);
                    if (list != null)
                        _segmentList.AddRange(list);
                }
                _wordList = new List<WordInfo>(wordInfos);
            }
            else
            {
                if (_inputText.EndsWith(_indexFlag))  //逗号空格分词
                {
                    string[] sources = _inputText.Replace(_indexFlag, "").Split(_splitFlag, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string source in sources)
                    {
                        _segmentList.Add(source);
                        var spells = SpellUtils.GetSpellSegment(source);
                        if (spells != null)
                            _segmentList.AddRange(spells);
                    }
                    
                }
                else if (_inputText.EndsWith(_indexCnName))  //拼音首字母分词
                {
                    string source = _inputText.Replace(_indexCnName, "");
                    _wordList = SpellUtils.GetCnSegment(source);
                }
                else  //flag分词
                {
                    string source = _inputText.Replace("|", "");
                    for (int i = 0; i < source.Length; i++)
                    {
                        string first = source[i].ToString();
                        _segmentList.Add(first);
                        int f = 1;
                        for (int j = i + 1; j < source.Length; j++)
                        {
                            string s = string.Format("{0}|{1}", first, source[j]);
                            _segmentList.Add(s);
                            first = s;
                            if (j == source.Length - 1)
                            {
                                f++;
                                j = i + f - 1;
                                first = source[i].ToString();
                            }
                        }
                    }
                }
            }
        }
        //DotLucene的分词器简单来说，就是实现Tokenizer的Next方法，把分解出来的每一个词构造为一个Token，因为Token是DotLucene分词的基本单位。
        public override Token Next()
        {
            if (_originalResult)
            {
                string retStr = _inputText;
                _inputText = null;
                if (retStr == null)
                {
                    return null;
                }
                return new Token(retStr, 0, retStr.Length);
            }
            if (!_customResult && _segmentList.Count > 0)
            {
                _position++;
                if (_position < _segmentList.Count)
                {
                    if (_position == _segmentList.Count - 1)
                    {
                        _position = -1;
                        _customResult = true;
                        if (_wordList != null && _wordList.Count > 0)
                            return new Token(_segmentList[_segmentList.Count - 1], 0, 0);
                        else
                            return new Token(_segmentList[_segmentList.Count - 1], 0, _inputText.Length);
                    }
                    return new Token(_segmentList[_position], 0, 0);
                }
            }
            if (_wordList != null && _wordList.Count > 0)
            {
                int length = 0;    //词汇的长度.
                int start = 0;     //开始偏移量.
                while (true)
                {
                    _position++;
                    if (_position < _wordList.Count)
                    {
                        if (_wordList[_position] != null)
                        {
                            length = _wordList[_position].Word.Length;
                            start = _wordList[_position].Position;
                            return new Token(_wordList[_position].Word, start, start + length);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            _inputText = null;
            return null;
        }

        /// <summary>
        /// 判断是否是Flag分词
        /// </summary>
        /// <returns></returns>
        private bool AnalyzInput()
        {
            if (_inputText.EndsWith(_indexFlag) || _inputText.EndsWith(_indexCnName))
                return true;
            bool result = false;
            for (int i = 0; i < _inputText.Length; i++)
            {
                if (i % 2 == 0)
                {
                    result = char.IsLetter(_inputText[i]);
                }
                else
                {
                    result = _inputText[i] == '|';
                }
                if (!result)
                    return result;
            }
            return result;
        }
    }
}
