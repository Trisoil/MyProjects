using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.LuceneNetSearcher.Spell
{
    public class SpellContainer
    {
        private Dictionary<char, WordToSpellNode> _dic;
        public static readonly SpellContainer Instance = new SpellContainer();

        private SpellContainer()
        { }

        public void Init()
        {
            LoadSpells();
        }

        private void LoadSpells()
        {
            string filePath = string.Format("{0}/Config/WordToSpellNode.xml", AppDomain.CurrentDomain.BaseDirectory);
            var root = XElement.Load(filePath);
            var words = from word in root.Elements("Word") select new WordToSpellNode() { Name = Convert.ToChar(word.Attribute("Name").Value), AllSpells = word.Element("AllSpells").Elements("AllSpell").Select(a => (string)a.Attribute("Name")).ToPerfectList(), FirstSpells = word.Element("FirstSpells").Elements("FirstSpell").Select(f => (string)f.Attribute("Name")).ToPerfectList() };
            _dic = words.ToPerfectDictionary(w => w.Name);
        }

        public WordToSpellNode this[char word]
        {
            get {
                if (_dic.ContainsKey(word))
                    return _dic[word];
                return null;
            }
        }
    }
}
