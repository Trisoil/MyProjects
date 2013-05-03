using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Net;
using System.Xml.XPath;
using System.IO;
using System.Xml.Serialization;

namespace CreatePushFile
{
    class PushTask
    {
        public string FileName { get; set;  }
        public int Copies { get; set; }
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("CreatePushFile <MP4ListFileName> <taskFileName>");
                return;
            }

            string mp4ListFile = args[0];
            string taskFileName = args[1];

            List<PushTask> tasks = GetTasks(mp4ListFile);

            List<PushTask> errorTasks = new List<PushTask>();

            string existingTasks = GetExistingTasks(taskFileName);

            Console.WriteLine("Creating push file ...{0}", taskFileName);

            using (TextWriter writer = new StreamWriter(taskFileName))
            {
                writer.WriteLine("<?xml version=\"1.0\">");
                writer.WriteLine("<tasklist>");

                if (!string.IsNullOrEmpty(existingTasks))
                {
                    writer.WriteLine(existingTasks);
                }

                foreach (PushTask task in tasks)
                {
                    Console.WriteLine("Creating {0} push task for file '{1}'", task.Copies, task.FileName);

                    string encodedFileName = HttpUtility.UrlEncode(task.FileName, Encoding.GetEncoding("GB2312"));
                    encodedFileName = encodedFileName.Replace("+", "%20");

                    string drag = GetDrag(encodedFileName);

                    if (string.IsNullOrEmpty(drag))
                    {
                        Console.WriteLine("Error: cannot get drag for {0}. ", task.FileName);
                        errorTasks.Add(task);
                        continue;
                    }

                    try
                    {
                        StringReader reader = new StringReader(drag);
                        XPathDocument doc = new XPathDocument(reader);

                        XPathNavigator navigator = doc.CreateNavigator();
                        XPathNodeIterator iterator = navigator.Select("/root/segments/segment");
                        while (iterator.MoveNext())
                        {
                            XPathNavigator current = iterator.Current;
                            string varid = current.GetAttribute("varid", current.NamespaceURI);
                            string index = current.GetAttribute("no", current.NamespaceURI);

                            string encodedVarid = varid.Replace("&", "&amp;");

                            writer.WriteLine("<task>");
                            writer.WriteLine("<rid>{0}</rid>", encodedVarid);
                            writer.WriteLine("<urlinfo>");
                            writer.WriteLine(
                                "<url>http://ccf.pptv.com/{0}/{1}</url>",
                                index,
                                encodedFileName);
                            writer.WriteLine("</urlinfo>");
                            writer.WriteLine("<priority>1</priority>");
                            writer.WriteLine("<aim_num>{0}</aim_num>", task.Copies);
                            writer.WriteLine("</task>");

                        }
                    }
                    catch(Exception ex)
                    {
                        Console.WriteLine("Failed to add task for {0}", task.FileName);
                    }
                }

                writer.WriteLine("</tasklist>");
            }

            WriteTasks(errorTasks, mp4ListFile);

            Console.WriteLine("Done.");

        }

        private static void WriteTasks(List<PushTask> tasks, string fileName)
        {
            Console.WriteLine("There are {0} remaining tasks.", tasks.Count);
            Console.WriteLine("Writing remaining tasks to {0} ...", fileName);

            try
            {
                File.Delete(fileName);
            }
            catch (System.Exception ex)
            {
            }

            using (FileStream fs = File.OpenWrite(fileName))
            {
                using (StreamWriter sw = new StreamWriter(fs, Encoding.GetEncoding("GB2312")))
                {
                    foreach (PushTask task in tasks)
                    {
                        sw.WriteLine(task.FileName);
                        sw.WriteLine(task.Copies);
                    }

                    sw.WriteLine();
                }
            }
        }

        private static string GetExistingTasks(string taskFileName)
        {
            if (File.Exists(taskFileName))
            {
                Console.WriteLine("{0} exists. Reading tasks ...", taskFileName);

                string allText = File.ReadAllText(taskFileName);

                int startPos = allText.IndexOf("<task>");
                if (startPos >= 0)
                {
                    int endPos = allText.LastIndexOf("</task>");

                    if (endPos >= 0)
                    {
                        string tasks = allText.Substring(startPos, endPos + 7 - startPos);
                        return tasks;
                    }
                }
            }

            return null;
        }

        private static List<PushTask> GetTasks(string fileName)
        {
            List<PushTask> list = new List<PushTask>();
            using(StreamReader reader = new StreamReader(fileName, Encoding.GetEncoding("GB2312")))
            {
                while (true)
                {
                    string mediaFileName = reader.ReadLine();
                    string copies = reader.ReadLine();

                    if (!string.IsNullOrEmpty(mediaFileName) && !string.IsNullOrEmpty(copies))
                    {
                        int copiesValue;
                        if (int.TryParse(copies, out copiesValue))
                        {
                            PushTask task = new PushTask();
                            task.FileName = mediaFileName;
                            task.Copies = copiesValue;
                            list.Add(task);
                        }
                        else
                        {
                            Console.WriteLine("{0} is not a valid integer.", copiesValue);
                            return null;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            return list;
        }

        private static string GetDrag(string fileName)
        {
            string url = "http://drag.synacast.com/" + fileName;

            try
            {
                Console.WriteLine("Downloading drag from url: {0}", url);
                WebClient client = new WebClient();
                string drag = client.DownloadString(url);
                return drag;
            }
            catch(Exception ex)
            {
                Console.WriteLine("Failed to get drag: {0}", ex.Message);
            }

            return null;
        }
    }
}
