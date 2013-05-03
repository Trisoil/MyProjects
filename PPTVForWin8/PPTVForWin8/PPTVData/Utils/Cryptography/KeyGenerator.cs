using System;
using System.Text;

namespace PPTVData
{
    public class KeyGenerator
    {
        private const uint ENCRYPT_ROUNDS = 32;
        private const uint DELTA = 0x9E3779B9;
        private const uint FINAL_SUM = 0xC6EF3720;
        private const uint BLOCK_SIZE = (4 << 1);
        private const uint BLOCK_SIZE_TWICE = ((4 << 1) << 1);
        private const uint BLOCK_SIZE_HALF = ((4 << 1) >> 1);

        static public string GetKey(DateTime time)
        {
            ulong time_now = (ulong)(time - (new DateTime(1970, 1, 1))).TotalSeconds;
            return GetKey(time_now);
        }

        static public string GetKey(ulong time_now)
        {
            string keypwd = "qqqqqww";
            byte[] key = System.Text.Encoding.UTF8.GetBytes(keypwd);
            byte[] key16 = new byte[16];
            for (int i = 0; i < key16.Length; i++) key16[i] = (byte)0;
            Array.Copy(key, key16, key.Length);
            string bytes_string = time_now.ToString("x");
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(bytes_string);
            byte[] bytes16 = new byte[16];
            for (int i = 0; i < bytes16.Length; i++) bytes16[i] = (byte)0;
            Array.Copy(bytes, bytes16, Math.Min(bytes.Length, bytes16.Length));

            TEncrypt(bytes16, key16);

            return StringToHex(bytes16);
        }

        static private void TEncrypt(byte[] bytes, byte[] key)
        {
            uint k0 = GetKeyFromStr(key);
            uint k1 = k0 << 8 | k0 >> 24;
            uint k2 = k0 << 16 | k0 >> 16;
            uint k3 = k0 << 24 | k0 >> 8;

            for (int i = 0; i + 16 <= bytes.Length; i += 16)
            {
                uint ascii0 = (uint)bytes[i + 0] << 0;
                uint ascii1 = (uint)bytes[i + 1] << 8;
                uint ascii2 = (uint)bytes[i + 2] << 16;
                uint ascii3 = (uint)bytes[i + 3] << 24;
                uint ascii4 = (uint)bytes[i + 4] << 0;
                uint ascii5 = (uint)bytes[i + 5] << 8;
                uint ascii6 = (uint)bytes[i + 6] << 16;
                uint ascii7 = (uint)bytes[i + 7] << 24;
                uint v0 = ascii0 | ascii1 | ascii2 | ascii3;
                uint v1 = ascii4 | ascii5 | ascii6 | ascii7;
                uint sum = 0;
                for (int j = 0; j < ENCRYPT_ROUNDS; j++)
                {
                    sum += DELTA;
                    v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
                    v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
                }
                bytes[i + 0] = (byte)((v0 >> 0) & 0xff);
                bytes[i + 1] = (byte)((v0 >> 8) & 0xff);
                bytes[i + 2] = (byte)((v0 >> 16) & 0xff);
                bytes[i + 3] = (byte)((v0 >> 24) & 0xff);
                bytes[i + 4] = (byte)((v1 >> 0) & 0xff);
                bytes[i + 5] = (byte)((v1 >> 8) & 0xff);
                bytes[i + 6] = (byte)((v1 >> 16) & 0xff);
                bytes[i + 7] = (byte)((v1 >> 24) & 0xff);
            }
        }

        static private uint GetKeyFromStr(byte[] bs)
        {
            byte[] key = new byte[4] { 0, 0, 0, 0 };
            for (int i = 0; i < bs.Length; i++)
                key[i % 4] ^= bs[i];
            uint ret = (uint)key[0] | ((uint)key[1] << 8) | ((uint)key[2] << 16) | ((uint)key[3] << 24);
            return ret;
        }

        static private string StringToHex(byte[] buffer)
        {
            StringBuilder sb = new StringBuilder(32);
            for (int i = 0; i < buffer.Length; i++)
            {
                byte b = (byte)(buffer[i] & (byte)0x0F);
                byte b1 = (byte)((buffer[i] >> 4) & (byte)0x0F);
                sb.Append((char)(b > 9 ? 'a' - 10 + b : '0' + b));
                sb.Append((char)(b1 > 9 ? 'a' - 10 + b1 : '0' + b1));
            }
            return sb.ToString();
        }
    }
}
