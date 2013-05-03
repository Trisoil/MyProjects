using System;
using System.Collections.Generic;
using System.Text;
using System.Web;
using System.Security.Cryptography;
using System.IO;

namespace Synacast.ProviderServices
{
    public class CryptHelp
    {
        private static byte[] Key192 = { 55, 103, 246, 79, 36, 99, 167, 3, 42, 5, 62, 83, 184, 7, 209, 13, 145, 23, 200, 58, 173, 10, 121, 222 };
        private static byte[] IV64 = { 55, 103, 246, 79, 36, 99, 167, 3 };

        private static readonly byte[] pKEY = HexStringToByteArray("DAEFE3161F3578E0DFDFABD28C9E2F567A27EE5F2F8A2C9B");
        private static readonly byte[] pIV ={ 1, 2, 3, 4, 5, 6, 7, 8 };
        private static TripleDESCryptoServiceProvider des = new TripleDESCryptoServiceProvider();


        static CryptHelp()
        {
            string key = System.Web.Configuration.WebConfigurationManager.AppSettings["AppKey"];
            if(!string.IsNullOrEmpty(key))
                Key192 = HexStringToByteArray(key);
            string iv = System.Web.Configuration.WebConfigurationManager.AppSettings["AppIV"];
            if (!string.IsNullOrEmpty(iv))
                IV64 = HexStringToByteArray(iv);

            des.Mode = System.Security.Cryptography.CipherMode.ECB;
            des.Mode = System.Security.Cryptography.CipherMode.CBC;
            des.Padding = System.Security.Cryptography.PaddingMode.PKCS7;
        }

        public static String EncryptTripleDES(String valueString)
        {
            if (valueString != "")
            {
                //定义TripleDES的Provider
                TripleDESCryptoServiceProvider triprovider = new TripleDESCryptoServiceProvider();
                //定义内存流
                MemoryStream memoryStream = new MemoryStream();
                //定义加密流
                CryptoStream cryptoStream = new CryptoStream(memoryStream, triprovider.CreateEncryptor(Key192, IV64), CryptoStreamMode.Write);
                //定义写IO流
                StreamWriter writerStream = new StreamWriter(cryptoStream);
                //写入加密后的字符流
                writerStream.Write(valueString);
                writerStream.Flush();
                cryptoStream.FlushFinalBlock();
                memoryStream.Flush();
                //返回加密后的字符串
                return (Convert.ToBase64String(memoryStream.GetBuffer(), 0, (int)memoryStream.Length));
            }
            return (null);
        }

        public static String DecryptTripleDES(String valueString)
        {
            if (valueString != "")
            {
                //定义TripleDES的Provider
                TripleDESCryptoServiceProvider triprovider = new TripleDESCryptoServiceProvider();
                //转换解密的字符串为二进制
                byte[] buffer = Convert.FromBase64String(valueString);
                //定义内存流
                MemoryStream memoryStream = new MemoryStream(buffer);
                //定义加密流
                CryptoStream cryptoStream = new CryptoStream(memoryStream, triprovider.CreateDecryptor(Key192, IV64), CryptoStreamMode.Read);
                //定义读IO流
                StreamReader readerStream = new StreamReader(cryptoStream);
                //返回解密后的字符串
                return (readerStream.ReadToEnd());
            }
            return (null);
        }

        public static string ToBase64String(byte[] buf)
        {
            return System.Convert.ToBase64String(buf);
        }
        public static byte[] FromBase64String(string s)
        {
            return System.Convert.FromBase64String(s);
        }

        public static byte[] ConvertStringToByteArray(String s)
        {
            return System.Text.Encoding.GetEncoding("utf-8").GetBytes(s);
        }

        public static string ConvertByteArrayToString(byte[] buf)
        {
            return System.Text.Encoding.GetEncoding("utf-8").GetString(buf);
        }

        /// <summary>
        /// 将byte数组转化为用16进制字符串的表示形式。如byte[]{255, 13, 16}转化后即为FF0D10
        /// </summary>
        /// <param name="buf">待转化的byte数组</param>
        /// <returns>转化后得到的字符串</returns>
        public static string ByteArrayToHexString(byte[] buf)
        {
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < buf.Length; i++)
            {
                sb.Append(buf[i].ToString("X").Length == 2 ? buf[i].ToString("X") : "0" + buf[i].ToString("X"));
            }

            return sb.ToString();
        }

        /// <summary>
        /// 将byte数组的16进制字符串表示形式转化为实际的byte数组。如"FF0D10"转化后即为byte[]{255, 13, 16}
        /// </summary>
        /// <param name="s">byte数组的16进制字符串表示形式</param>
        /// <returns>实际的byte数组</returns>
        public static byte[] HexStringToByteArray(string s)
        {
            Byte[] buf = new byte[s.Length / 2];

            for (int i = 0; i < buf.Length; i++)
            {
                buf[i] = (byte)(chr2hex(s.Substring(i * 2, 1)) * 0x10 + chr2hex(s.Substring(i * 2 + 1, 1)));
            }

            return buf;
        }

        private static byte chr2hex(string chr)
        {
            switch (chr)
            {
                case "0":
                    return 0x00;
                case "1":
                    return 0x01;
                case "2":
                    return 0x02;
                case "3":
                    return 0x03;
                case "4":
                    return 0x04;
                case "5":
                    return 0x05;
                case "6":
                    return 0x06;
                case "7":
                    return 0x07;
                case "8":
                    return 0x08;
                case "9":
                    return 0x09;
                case "A":
                    return 0x0a;
                case "B":
                    return 0x0b;
                case "C":
                    return 0x0c;
                case "D":
                    return 0x0d;
                case "E":
                    return 0x0e;
                case "F":
                    return 0x0f;
            }

            return 0x00;
        }

        /// <summary>
        /// 使用utf-8编码进行Encode解码
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static string UrlEncode(string str)
        {
            return HttpUtility.UrlEncode(str, System.Text.Encoding.GetEncoding("utf-8"));
        }

        /// <summary>
        /// 使用utf-8编码进行Decode解码
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static string UrlDecode(string str)
        {
            return HttpUtility.UrlDecode(str, System.Text.Encoding.GetEncoding("utf-8"));
        }


        public static string Encrypt(byte[] pKEY, byte[] pIV, string strTobeEnCrypted)
        {
            return ToBase64String(Encrypt(pKEY, pIV, ConvertStringToByteArray(strTobeEnCrypted)));
        }

        public static string Encrypt(string key, string iv, string strTobeEnCrypted)
        {
            byte[] pKEY = HexStringToByteArray(key);
            byte[] pIV = HexStringToByteArray(iv);
            return Encrypt(pKEY, pIV, strTobeEnCrypted);
        }

        private static byte[] Encrypt(byte[] KEY, byte[] IV, byte[] TobeEncrypted)
        {
            if (TobeEncrypted == null)
            {
                return null;
            }

            byte[] Encrypted = null;

            byte[] tmpiv ={ 0, 1, 2, 3, 4, 5, 6, 7 };
            for (int ii = 0; ii < 8; ii++)
            {
                tmpiv[ii] = IV[ii];
            }
            byte[] tmpkey ={ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 };
            for (int ii = 0; ii < 24; ii++)
            {
                tmpkey[ii] = KEY[ii];
            }

            try
            {
                ICryptoTransform tridesencrypt = des.CreateEncryptor(tmpkey, tmpiv);
                Encrypted = tridesencrypt.TransformFinalBlock(TobeEncrypted, 0, TobeEncrypted.Length);
                des.Clear();
            }
            catch //(Exception ex)
            {
                //throw ex;
                return null;
            }
            
            return Encrypted;
        }

        public static string Decrypt(byte[] pKEY, byte[] pIV, string strTobeDeCrypted)
        {
            return ConvertByteArrayToString(Decrypt(pKEY, pIV, FromBase64String(strTobeDeCrypted)));
        }

        public static string Decrypt(string key, string iv, string strTobeDeCrypted)
        {
            byte[] pKEY = HexStringToByteArray(key);
            byte[] pIV = HexStringToByteArray(iv);
            return Decrypt(pKEY, pIV, strTobeDeCrypted);
        }

        private static byte[] Decrypt(byte[] KEY, byte[] IV, byte[] TobeDecrypted)
        {
            if (TobeDecrypted == null)
            {
                return null;
            }

            byte[] Decrypted = null;

            byte[] tmpiv ={ 0, 1, 2, 3, 4, 5, 6, 7 };
            for (int ii = 0; ii < 8; ii++)
            {
                tmpiv[ii] = IV[ii];
            }
            byte[] tmpkey ={ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 };
            for (int ii = 0; ii < 24; ii++)
            {
                tmpkey[ii] = KEY[ii];
            }

            try
            {
                ICryptoTransform tridesdecrypt = des.CreateDecryptor(tmpkey, tmpiv);
                Decrypted = tridesdecrypt.TransformFinalBlock(TobeDecrypted, 0, TobeDecrypted.Length);
                des.Clear();
            }
            catch //(Exception ex)
            {
                //throw ex;
                return null;
            }

            return Decrypted;
        }

    }
}
