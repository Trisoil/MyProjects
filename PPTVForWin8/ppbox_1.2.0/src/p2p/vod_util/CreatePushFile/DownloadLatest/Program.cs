using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Xml.XPath;

namespace DownloadLatest
{
    class Program
    {
        public const string LatestResourceUrl = "http://122.224.72.27/Services/FileListXml.ashx";

        public const int MinimumDurationInSeconds = 600;

        public const int DefaultPushCount = 1000;

        public const int MinimumBitRate = 500;

        public const string LatestDateTimeFile = "LatestTime.txt";

        private static DateTime LatestTimestamp = DateTime.MinValue;

        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("DownloadLatest <OutputMP4ListFile>");
                Environment.Exit(-1);
                return;
            }

            string outputFile = args[0];

            Console.WriteLine("Output MP4 list file: {0}", outputFile);

            Console.WriteLine("Reading latest timestamp from {0} ...", LatestDateTimeFile);
            try
            {
                string latestTimeString = File.ReadAllText(LatestDateTimeFile);
                LatestTimestamp = DateTime.Parse(latestTimeString);
            }
            catch (System.Exception ex)
            {
                Console.WriteLine("Failed to get latest timestamp. Set to minimum.");
                LatestTimestamp = DateTime.MinValue;
            }

            string resourcesXml = DownloadResources();
            if (string.IsNullOrEmpty(resourcesXml))
            {
                Console.WriteLine("Error...exit.");
                Environment.Exit(-1);
                return;
            }

            DateTime currentTimestamp = DateTime.MinValue;

            List<string> fileNames = new List<string>();

            StringReader reader = new StringReader(resourcesXml);
            XPathDocument doc = new XPathDocument(reader);

            XPathNavigator navigator = doc.CreateNavigator();
            XPathNodeIterator iterator = navigator.Select("/root/Files/File");
            while (iterator.MoveNext())
            {
                XPathNavigator current = iterator.Current;
                string name = current.GetAttribute("nm", current.NamespaceURI);
                string duration = current.GetAttribute("se", current.NamespaceURI);
                string creationTime = current.GetAttribute("cT", current.NamespaceURI);
                string br = current.GetAttribute("br", current.NamespaceURI);

                int durationInSeconds = 0;
                int.TryParse(duration, out durationInSeconds);

                DateTime creationDateTime = DateTime.Parse(creationTime);

                int bitRate = 0;
                int.TryParse(br, out bitRate);

                if (ShouldPush(name, durationInSeconds, creationDateTime, bitRate))
                {
                    fileNames.Add(name);
                }

                if (currentTimestamp < creationDateTime)
                {
                    currentTimestamp = creationDateTime;
                }
            }

            List<string> lines = new List<string>();

            try
            {
                if (File.Exists(outputFile))
                {
                    string[] strs = File.ReadAllLines(outputFile, Encoding.GetEncoding("GB2312"));
                    foreach(string str in strs)
                    {
                        if (!string.IsNullOrEmpty(str))
                        {
                            string trimmed = str.Trim();
                            if (!string.IsNullOrEmpty(trimmed))
                            {
                                lines.Add(str);
                            }
                        }
                    }
                }

                File.Delete(outputFile);
            }
            catch (System.Exception ex)
            {
            	
            }

            Console.WriteLine("Writing {0} new MP4 files to MP4 list file {1} ...", fileNames.Count, outputFile);

            using (FileStream fs = File.OpenWrite(outputFile))
            {
                using (StreamWriter sw = new StreamWriter(fs, Encoding.GetEncoding("GB2312")))
                {
                    // write old lines
                    foreach(string line in lines)
                    {
                        sw.WriteLine(line);
                    }

                    foreach (string name in fileNames)
                    {
                        sw.WriteLine(name);
                        sw.WriteLine(DefaultPushCount);
                    }

                    sw.WriteLine();
                }
            }

            Console.WriteLine("Writing timestamp {0} to {1} ...", currentTimestamp, LatestDateTimeFile);

            try
            {
                File.WriteAllText(LatestDateTimeFile, currentTimestamp.ToString());
            }
            catch (System.Exception ex)
            {
                Console.WriteLine("Error: cannot write current timestamp to {0}. ", LatestDateTimeFile);
            	
            }

            Console.WriteLine("Done.");
        }

        private static bool ShouldPush(string name, int durationInSeconds, DateTime creationTime, int bitRate)
        {
            if (name.Contains("[mobile]"))
            {
                // mobile resources
                return false;
            }

            if (durationInSeconds < MinimumDurationInSeconds)
            {
                return false;
            }

            if (creationTime < LatestTimestamp)
            {
                return false;
            }

            if (bitRate < MinimumBitRate)
            {
                return false;
            }

            return true;
        }

        private static string DownloadResources()
        {
            try
            {
                Console.WriteLine("Downloading resources from url: {0}", LatestResourceUrl);
                WebClient client = new WebClient();
                client.Encoding = Encoding.UTF8;
                string resources = client.DownloadString(LatestResourceUrl);
                return resources;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed to get drag: {0}", ex.ToString());
            }

            return null;
        }
    }
}
