using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.LuceneNetSearcher.Spell
{
    public class WordToSpellNode
    {
        public char Name { get; set; }

        public List<string> AllSpells { get; set; }

        public List<string> FirstSpells { get; set; }
    }
}
