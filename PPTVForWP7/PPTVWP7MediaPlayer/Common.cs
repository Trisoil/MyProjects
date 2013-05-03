using System;
using System.Text;

namespace PPTVWP7MediaPlayer
{
    public class Common
    {
        public static string Decode(string url, string key)
        {
            byte[] bin = Convert.FromBase64String(url);
            byte[] byteKey = Encoding.UTF8.GetBytes(key);
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] - (int)byteKey[i % byteKey.Length]);

            char[] utf8Chars = new char[Encoding.UTF8.GetCharCount(bout, 0, bout.Length)];
            Encoding.UTF8.GetChars(bout, 0, bout.Length, utf8Chars, 0);
            return new string(utf8Chars);
        }

        public static string Encode(string url, string key)
        {
            if (string.IsNullOrEmpty(url))
                return string.Empty;
            byte[] bin = Encoding.UTF8.GetBytes(url);
            byte[] byteKey = Encoding.UTF8.GetBytes(key);
            byte[] bout = new byte[bin.Length];
            for (int i = 0; i < bin.Length; i++)
                bout[i] = (byte)((int)bin[i] + (int)byteKey[i % byteKey.Length]);
            return Convert.ToBase64String(bout);
        }
    }
}
