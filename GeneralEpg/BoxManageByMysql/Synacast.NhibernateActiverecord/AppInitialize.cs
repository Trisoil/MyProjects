using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Reflection;
using System.Xml.Serialization;
using System.IO;

namespace Synacast.NhibernateActiveRecord
{
    public class AppInitialize
    {
        public static Assembly[] LoadAssemblies(string file)
        {
            XmlRootAttribute att = new XmlRootAttribute("assemblies");
            XmlSerializer xs = new XmlSerializer(typeof(List<AssemblyConfig>), att);
            List<AssemblyConfig> list = null;
            using (FileStream stream = new FileStream(file, FileMode.Open))
            {
                list = (List<AssemblyConfig>)xs.Deserialize(stream);
            }
            Assembly[] assemblies = new Assembly[list.Count];
            int i = 0;
            list.ForEach(delegate(AssemblyConfig config)
            {
                assemblies[i] = Assembly.Load(config.Name);
                i++;
            });
            return assemblies;
        }
    }

    [XmlType("assembly")]
    public sealed class AssemblyConfig
    {
        private string _name;

        [XmlAttribute("name")]
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }
    }
}
