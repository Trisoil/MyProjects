using System;
using System.Net;
using System.Windows.Threading;

using PPTVData.Entity;
using System.Text;
using System.IO;
using System.Reflection;
using System.Windows;

namespace PPTVData.Factory
{
    public class LoginFactory : HttpFactoryBase<UserStateInfo>
    {
        private static string[] KEY = { "70706C6976656F6B",
			"15B9FDAEDA40F86BF71C73292516924A294FC8BA31B6E9EA",
			"29028A7698EF4C6D3D252F02F4F79D5815389DF18525D326",
			"D046E6B6A4A85EB6C44C73372A0D5DF1AE76405173B3D5EC",
			"435229C8F79831131923F18C5DE32F253E2AF2AD348C4615",
			"9B2915A72F8329A2FE6B681C8AAE1F97ABA8D9D58576AB20",
			"B3B0CD830D92CB3720A13EF4D93B1A133DA4497667F75191",
			"AD327AFB5E19D023150E382F6D3B3EB5B6319120649D31F8",
			"C42F31B008BF257067ABF115E0346E292313C746B3581FB0",
			"529B75BAE0CE2038466704A86D985E1C2557230DDF311ABC",
			"8A529D5DCE91FEE39E9EE9545DF42C3D9DEC2F767C89CEAB", };

        int _keyIndex = 1;

        public LoginFactory()
        {
        }

        protected override void _webClient_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
        {
            try
            {
                string responseStr = ParseLoginResponse(e.Result, _keyIndex);
                string[] temp = responseStr.Split('&');
                Utils._userStateInfo = new UserStateInfo();
                //(用户名&VIP标识&头像地址&等级&积分&经验值&用户状态&时间戳&过期时间&VIP会员到期时间&VIP会员类型& MD5&用户Token)
                for (int i = 0; i < temp.Length; i++)
                {
                    if (i == 0)
                    {
                        //会员名
                        Utils._userStateInfo.UserName = temp[i];
                    }
                    else if (i == 1)
                    {
                        //VIP标识
                        try
                        {
                            Utils._userStateInfo.VIP = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.VIP = -1;
                        }
                        
                    }
                    else if (i == 2)
                    {
                        //头像地址
                        Utils._userStateInfo.FaceUrl = temp[i];
                    }
                    else if (i == 3)
                    {
                        //等级
                        try
                        {
                            Utils._userStateInfo.Degree = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {

                            Utils._userStateInfo.Degree = -1;
                        }
                        
                    }
                    else if (i == 4)
                    {
                        //积分
                        try
                        {
                            Utils._userStateInfo.Mark = Convert.ToDouble(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.Mark = -1.0;
                        }
                        
                    }
                    else if (i == 5)
                    {
                        //经验值
                        try
                        {
                            Utils._userStateInfo.Expence = Convert.ToDouble(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.Expence = -1;
                        }
                        
                    }
                    else if (i == 6)
                    {
                        //用户状态
                        try
                        {
                            Utils._userStateInfo.UserState = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.UserState = -1;
                        }
                        
                    }
                    else if (i == 7)
                    {
                        //时间
                        try
                        {
                            Utils._userStateInfo.Date = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.Date = -1;
                        }
                        
                    }
                    else if (i == 8)
                    {
                        //过期时间
                        try
                        {
                            Utils._userStateInfo.OutDate = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.OutDate = 0;
                        }
                        
                    }
                    else if (i == 9)
                    {
                        //VIP到期时间
                        try
                        {
                            Utils._userStateInfo.VIPDate = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.VIPDate = 0;
                        }
                        
                    }
                    else if (i == 10)
                    {
                        //VIP会员的类型
                        try
                        {
                            if (Utils._userStateInfo.VIP == 1)
                                Utils._userStateInfo.VIPType = Convert.ToInt32(temp[i]);
                            else
                            {
                                Utils._userStateInfo.VIPType = -1;//普通会员-1
                            }
                        }
                        catch (Exception)
                        {
                            Utils._userStateInfo.VIPType = -1;
                        }
                    }
                    else if (i == 11)
                    {
                        //MD5
                        Utils._userStateInfo.MD5 = temp[i];
                    }
                    else if (i == 12)
                    {
                        //Token
                        Utils._userStateInfo.Token = temp[i];
                    }
                    else if (i == 13)
                    {
                        Utils._userStateInfo.P13 = temp[i];
                    }
                    else if (i == 14)
                    {
                        Utils._userStateInfo.P14 = temp[i];
                    }
                    else if (i == 15)
                    {
                        Utils._userStateInfo.P15 = temp[i];
                    }
                }
                if (HttpSucessHandler != null)
                    Deployment.Current.Dispatcher.BeginInvoke(
                        HttpSucessHandler, this, new HttpFactoryArgs<UserStateInfo>(string.Empty, Utils._userStateInfo));
            }
            catch (Exception ex)
            {
                if (HttpFailHandler != null)
                    Deployment.Current.Dispatcher.BeginInvoke(
                    HttpFailHandler, this, new HttpFactoryArgs<UserStateInfo>(ex.Message, null));
            }
        }

        protected override string CreateUri(params object[] paras)
        {
            var userInfo = paras[0] as UserInfo;
            return GetLoginUrl(userInfo.UserName, userInfo.PassWord);
        }

        protected override UserStateInfo AnalysisData(System.Xml.XmlReader reader)
        {
            return null;
        }

        public static byte[] HexToBytes(string str)
        {
            if (str.Length == 0 || str.Length % 2 != 0)
                return new byte[0];

            byte[] buffer = new byte[str.Length / 2];
            char c;
            for (int bx = 0, sx = 0; bx < buffer.Length; ++bx, ++sx)
            {
                // Convert first half of byte
                c = str[sx];
                buffer[bx] = (byte)((c > '9' ? (c > 'Z' ? (c - 'a' + 10) : (c - 'A' + 10)) : (c - '0')) << 4);

                // Convert second half of byte
                c = str[++sx];
                buffer[bx] |= (byte)(c > '9' ? (c > 'Z' ? (c - 'a' + 10) : (c - 'A' + 10)) : (c - '0'));
            }

            return buffer;
        }

        public string GetLoginUrl(string userName, string password)
        {
            userName.Trim();
            password.Trim();
            if (userName == "" || password == "")
            {
                return "";
            }

            string userParam = HttpUtility.UrlEncode(userName) + "&&"
                + HttpUtility.UrlEncode(password) + "&&"
                + HttpUtility.UrlEncode(Utils.DeviceUniqueId);

            int randomKey = (new System.Random()).Next(99999);
            randomKey += 10000;  // 10000 - 109999
            int keyIndex = ((randomKey / 10000) % 11);
            _keyIndex = keyIndex;
            byte[] bufferOut = null;

            /*
            PaddedBufferedBlockCipher cipher = null;
            cipher = new PaddedBufferedBlockCipher(
                new CbcBlockCipher(new DesEdeEngine()), new Pkcs7Padding());

            KeyParameter keyParameter = new KeyParameter(HexToBytes(TripleDESKey[keyIndex]));
            ParametersWithIV parameters = new ParametersWithIV(keyParameter, HexToBytes(TripleDESKey[0]));
            cipher.Init(true, parameters);
            bufferOut = cipher.DoFinal(System.Text.Encoding.UTF8.GetBytes(userParam));
            */

            BroccoliProducts.DESCrytography.TripleDES(System.Text.Encoding.UTF8.GetBytes(userParam),
                   ref bufferOut, HexToBytes(KEY[keyIndex]), HexToBytes(KEY[0]), true);

            userParam = Convert.ToBase64String(bufferOut);

            return string.Format("http://passport.pptv.com/ClientXMLLoginV2.do?infoValue={0}&index={1,6:000000}", HttpUtility.UrlEncode(userParam),
                randomKey);
        }

        public static string ParseLoginResponse(string responseDecrpt, int keyIndex)
        {
            byte[] response = Convert.FromBase64String(responseDecrpt);
            string t = System.Text.Encoding.UTF8.GetString(response, 0, response.Length);
            byte[] bufferOut = null;
            BroccoliProducts.DESCrytography.TripleDES(response, ref bufferOut, HexToBytes(KEY[keyIndex]), HexToBytes(KEY[0]), false);
            string userParam = System.Text.Encoding.UTF8.GetString(bufferOut, 0, bufferOut.Length);
            return userParam;
        }

        ///<summary>Convert a string of hex digits (ex: E4 CA B2) to a byte array. </summary>
        ///<param name="s">The string containing the hex digits (with or without spaces).</param>
        ///<returns>Returns an array of bytes.</returns>
        private byte[] HexStringToByteArray(string s)
        {
            s = s.Replace("   ", " ");
            byte[] buffer = new byte[s.Length / 2];
            for (int i = 0; i < s.Length; i += 2)
                buffer[i / 2] = (byte)Convert.ToByte(s.Substring(i, 2), 16);
            return buffer;
        }
    }
    //CNMTC102&0&http://face.passport.pplive.com/ppface_black.jpg&1&25000&000&0&1329101967047&6000&1329101967049&&2BDA6ECA0D410496BAC527CEE11944DC&n5SNfKO/cWV/nYCmhYB2oamVnMqhlJm5lpmeqH2vfGyGiJG5oqepgp+YoYOWs6+8fXR2oISyjcmOl6DFnKaZx5y2jomTg2RlgZ52cQ==&0&0&-1
    //holyenzo&1&http://face.passport.pplive.com/ppface_black.jpg&3&37000&889&0&1329103779573&6000&1354550400000&0&1A19AF1D917B8DC63F6298E5B5AE203F&naSVeaOZdWV/nYCmlZd6xrinpce5vYl9k8CKnX2JgLCIiJW4oM2zgqGopYOZwq93fXR2qoGjgb2OvZTKm7yZyJ2Pfc6TqoWqgXuScQ==&1&1&13
    //用户名&VIP标识&头像地址&等级&积分&经验值&用户状态&时间戳&过期时间&VIP会员到期时间&VIP会员类型& MD5&用户Token&同步状态&开放状态
    public class UserStateInfo
    {
        //public string this[int index]
        //{
        //    get { return ""; }
        //}
        public string UserName { get; set; }//用户名
        public int VIP { get; set; }//VIP标识
        public string FaceUrl { get; set; }//头像地址
        public int Degree { get; set; }//等级
        public double Mark { get; set; }//积分
        public double Expence { get; set; }//经验值
        public int UserState { get; set; }//用户状态
        public long Date { get; set; }//时间戳
        public long OutDate { get; set; }//过期时间
        public long VIPDate { get; set; }//会员过期时间
        public int VIPType { get; set; }//VIP会员类型
        public string MD5 { get; set; }
        public string Token { get; set; }
        public string P13 { get; set; }//第13值
        public string P14 { get; set; }//第14值
        public string P15 { get; set; }//第15值
    }
}
