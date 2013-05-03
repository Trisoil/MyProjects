using System;
using System.Net;

using System.Text;
using System.IO;
using System.Reflection;
using System.Windows;
using Windows.UI.Core;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class LoginFactory : HttpFactoryBase<UserStateInfo>
    {
        int _keyIndex = 1;

        protected override string CreateUri(params object[] paras)
        {
            return GetLoginUrl(paras[0].ToString() , paras[1].ToString());
        }

        public string GetLoginUrl(string userName, string password)
        {
            if (string.IsNullOrEmpty(userName)
                || string.IsNullOrEmpty(password))
                return string.Empty;

            var userParam = WebUtility.UrlEncode(userName) + "&&"
                + WebUtility.UrlEncode(password) + "&&"
                + WebUtility.UrlEncode(DeviceInfoFactory.Instance.DataInfos[0].DeviceId);

            int randomKey = (new System.Random()).Next(99999);
            randomKey += 10000;  // 10000 - 109999
            int keyIndex = ((randomKey / 10000) % 11);
            _keyIndex = keyIndex;
            byte[] bufferOut = null;

            /*
            PaddedBufferedBlockCipher cipher = null;
            cipher = new PaddedBufferedBlockCipher(
                new CbcBlockCipher(new DesEdeEngine()), new Pkcs7Padding());

            KeyParameter keyParameter = new KeyParameter(Utils.HexToBytes(TripleDESKey[keyIndex]));
            ParametersWithIV parameters = new ParametersWithIV(keyParameter, Utils.HexToBytes(TripleDESKey[0]));
            cipher.Init(true, parameters);
            bufferOut = cipher.DoFinal(System.Text.Encoding.UTF8.GetBytes(userParam));
            */

            BroccoliProducts.DESCrytography.TripleDES(System.Text.Encoding.UTF8.GetBytes(userParam),
                   ref bufferOut, DataCommonUtils.HexToBytes(KEY[keyIndex]), DataCommonUtils.HexToBytes(KEY[0]), true);

            userParam = Convert.ToBase64String(bufferOut);

            return string.Format("http://passport.pptv.com/ClientXMLLoginV2.do?infoValue={0}&index={1,6:000000}", WebUtility.UrlEncode(userParam),
                randomKey);
        }

        protected override void _webClient_DownloadStringCompleted(object sender, HttpCompletedEventArgs e)
        {
            try
            {
                var result = string.Empty;
                using (var reader = new StreamReader(e.Result))
                {
                    result = reader.ReadToEnd();
                }

                string responseStr = ParseLoginResponse(result, _keyIndex);
                string[] temp = responseStr.Split('&');
                var userStateInfo = new UserStateInfo();
                //(用户名&VIP标识&头像地址&等级&积分&经验值&用户状态&时间戳&过期时间&VIP会员到期时间&VIP会员类型& MD5&用户Token)
                for (int i = 0; i < temp.Length; i++)
                {
                    if (i == 0)
                    {
                        //会员名
                        userStateInfo.UserName = temp[i];
                    }
                    else if (i == 1)
                    {
                        //VIP标识
                        try
                        {
                            userStateInfo.VIP = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.VIP = -1;
                        }

                    }
                    else if (i == 2)
                    {
                        //头像地址
                        userStateInfo.FaceUrl = temp[i];
                    }
                    else if (i == 3)
                    {
                        //等级
                        try
                        {
                            userStateInfo.Degree = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {

                            userStateInfo.Degree = -1;
                        }

                    }
                    else if (i == 4)
                    {
                        //积分
                        try
                        {
                            userStateInfo.Mark = Convert.ToDouble(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.Mark = -1.0;
                        }

                    }
                    else if (i == 5)
                    {
                        //经验值
                        try
                        {
                            userStateInfo.Expence = Convert.ToDouble(temp[i]) * 100.0;  //经验值*100
                        }
                        catch (Exception)
                        {
                            userStateInfo.Expence = -1;
                        }

                    }
                    else if (i == 6)
                    {
                        //用户状态
                        try
                        {
                            userStateInfo.UserState = Convert.ToInt32(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.UserState = -1;
                        }

                    }
                    else if (i == 7)
                    {
                        //时间
                        try
                        {
                            userStateInfo.Date = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.Date = -1;
                        }

                    }
                    else if (i == 8)
                    {
                        //过期时间
                        try
                        {
                            userStateInfo.OutDate = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.OutDate = 0;
                        }

                    }
                    else if (i == 9)
                    {
                        //VIP到期时间
                        try
                        {
                            userStateInfo.VIPDate = Convert.ToInt64(temp[i]);
                        }
                        catch (Exception)
                        {
                            userStateInfo.VIPDate = 0;
                        }

                    }
                    else if (i == 10)
                    {
                        //VIP会员的类型
                        try
                        {
                            if (userStateInfo.VIP == 1)
                                userStateInfo.VIPType = Convert.ToInt32(temp[i]);
                            else
                            {
                                userStateInfo.VIPType = -1;//普通会员-1
                            }
                        }
                        catch (Exception)
                        {
                            userStateInfo.VIPType = -1;
                        }
                    }
                    else if (i == 11)
                    {
                        //MD5
                        userStateInfo.MD5 = temp[i];
                    }
                    else if (i == 12)
                    {
                        //Token
                        userStateInfo.Token = temp[i];
                    }
                    else if (i == 13)
                    {
                        userStateInfo.P13 = temp[i];
                    }
                    else if (i == 14)
                    {
                        userStateInfo.P14 = temp[i];
                    }
                    else if (i == 15)
                    {
                        userStateInfo.P15 = temp[i];
                    }
                }
                if (HttpSucessHandler != null)
                    _dispatcher.BeginInvoke(() =>
                    {
                        HttpSucessHandler(this, new HttpFactoryArgs<UserStateInfo>(string.Empty, userStateInfo));
                    });
            }
            catch (Exception ex)
            {
                if (HttpFailHandler != null)
                    _dispatcher.BeginInvoke(() =>
                    {
                        HttpFailHandler(this, new HttpFactoryArgs<UserStateInfo>(ex.Message, null));
                    });
            }
            finally
            {
                Cancel();
            }
        }

        public static string ParseLoginResponse(string responseDecrpt, int keyIndex)
        {
            byte[] response = Convert.FromBase64String(responseDecrpt);
            string t = System.Text.Encoding.UTF8.GetString(response, 0, response.Length);
            byte[] bufferOut = null;
            BroccoliProducts.DESCrytography.TripleDES(response, ref bufferOut, DataCommonUtils.HexToBytes(KEY[keyIndex]), DataCommonUtils.HexToBytes(KEY[0]), false);
            string userParam = System.Text.Encoding.UTF8.GetString(bufferOut, 0, bufferOut.Length);
            return userParam;
        }
    }
    
}
