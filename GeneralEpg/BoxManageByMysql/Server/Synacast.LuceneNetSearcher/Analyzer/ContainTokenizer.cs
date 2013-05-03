using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Synacast.LuceneNetSearcher.Analyzer
{
    using Lucene.Net.Analysis;

    public class ContainTokenizer : Tokenizer 
    {
        string _InputText;
        List<string> _segmentList = new List<string>();
        int _position = -1;

        public ContainTokenizer(TextReader input)
            : base(input)
        {
            _InputText = base.input.ReadToEnd();
            if (string.IsNullOrEmpty(_InputText))
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
                    _InputText = inputStr.ToString();
                }
            }

            if (!string.IsNullOrEmpty(_InputText))
            {
                string source = _InputText.Replace("|", "");
                for (int i = 0; i < source.Length; i++)
                {
                    string first = source[i].ToString();
                    _segmentList.Add(first);
                    for (int j = i + 1; j < source.Length; j++)
                    {
                        string s = string.Format("{0}|{1}", first, source[j]);
                        _segmentList.Add(s);
                        first = s;
                    }
                }
            }
        }

        public override Token Next()
        {
            _position++;
            if (_position < _segmentList.Count)
            {
                if (_position == _segmentList.Count - 1)
                    return new Token(_segmentList[_position], 0, _InputText.Length);
                return new Token(_segmentList[_position], 0, 0);
            }
            _InputText = null;
            return null;
        }
    }
}
