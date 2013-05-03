using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace Synacast.BoxManage.Core.Utils
{
    using ICSharpCode.SharpZipLib.GZip;

    public class GZipStressUtils<T> where T : class
    {
        /// <summary>
        /// Gzip压缩缓存文件
        /// </summary>
        /// <param name="filePath">缓存文件路径</param>
        /// <returns>压缩后的二进制</returns>
        public static byte[] GZipStress(string filePath)
        {
            string path = filePath;
            if (!File.Exists(path))
            {
                path = string.Format("{0}/Config/{1}.boxcache", AppDomain.CurrentDomain.BaseDirectory, typeof(T).FullName);
            }
            using (FileStream ms = new FileStream(path, FileMode.Open))
            {
                using (MemoryStream stream = new MemoryStream())
                {
                    GZipOutputStream outStream = new GZipOutputStream(stream);
                    int readed;
                    byte[] buffer = new byte[2048];
                    do
                    {
                        readed = ms.Read(buffer, 0, buffer.Length);
                        outStream.Write(buffer, 0, readed);
                    }
                    while (readed != 0);
                    outStream.Flush();
                    outStream.Finish();
                    return stream.GetBuffer();
                }
            }
        }

        /// <summary>
        /// Gzip解压缓存文件
        /// </summary>
        /// <param name="buffer">待解压的缓存数据</param>
        /// <returns>解压后的对象集合</returns>
        public static void GZipDeStress(byte[] buffer)
        {
            using (FileStream fs = new FileStream(string.Format("{0}/Config/syncbuffer.boxcache", AppDomain.CurrentDomain.BaseDirectory), FileMode.Create))
            {
                fs.Write(buffer, 0, buffer.Length);
            }
            using (FileStream fs = new FileStream(string.Format("{0}/Config/syncbuffer.boxcache", AppDomain.CurrentDomain.BaseDirectory), FileMode.Open))
            {
                using (GZipInputStream inStream = new GZipInputStream(fs))
                {
                    using (FileStream deStream = new FileStream(string.Format("{0}/Config/{1}.boxcache", AppDomain.CurrentDomain.BaseDirectory,typeof(T).FullName), FileMode.Create))
                    {
                        int num;
                        byte[] buffer2 = new byte[100];
                        while ((num = inStream.Read(buffer2, 0, buffer2.Length)) != 0)
                        {
                            if (num == buffer2.Length)
                            {
                                deStream.Write(buffer2, 0, buffer2.Length);
                            }
                            else
                            {
                                for (int i = 0; i < num; i++)
                                {
                                    deStream.WriteByte(buffer2[i]);
                                }
                            }
                        }
                    }
                }
            }
        }

        [Obsolete("Please use other same name function!")]
        public static byte[] GZipStress(T obj)
        {
            BinaryFormatter formatter = new BinaryFormatter();
            using (MemoryStream ms = new MemoryStream())
            {
                formatter.Serialize(ms, obj);
                ms.Seek(0, SeekOrigin.Begin);
                MemoryStream stream = new MemoryStream();
                GZipOutputStream outStream = new GZipOutputStream(stream);
                int readed;
                byte[] buffer = new byte[2048];
                do
                {
                    readed = ms.Read(buffer, 0, buffer.Length);
                    outStream.Write(buffer, 0, readed);
                }
                while (readed != 0);
                outStream.Flush();
                outStream.Finish();
                return stream.GetBuffer();
            }
        }
    }
}
