//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.IO.IsolatedStorage;
//using System.IO;
//using PPTVData;
//using PPTVData.Entity;
//using PPTVData.Log;

//namespace PPTVData.Factory
//{
//    public class FavourFactory
//    {
//        string _filePath = "Favours.txt";
//        public void AddFavours(ChannelInfo channelInfo)
//        {
//            try
//            {
//                string data = "";
//                List<ChannelInfo> channelInfos = GetFavours();//读取之前本地保存的数据
//                //遍历是否同一部收藏影片
//                for (int i = 0; i < channelInfos.Count; i++)
//                {
//                    if (channelInfos[i].VID == channelInfo.VID)
//                        channelInfos.RemoveAt(i);
//                }

//                channelInfos.Add(channelInfo);

//                // 每行格式: <VID>|||||<Title>|||||<PlayLink>|||||<ImgUrl>|||||<IsHD>|||||<IsRecommend>|||||<IsNew>|||||<Is3D>|||||<Bitrate>|||||<ResolutionWidth>|||||<ResolutionHeight>|||||<TypeID>
//                string fileName = _filePath;
//                using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
//                {
//                    using (IsolatedStorageFileStream isfs = new IsolatedStorageFileStream(fileName, FileMode.Create, isf))
//                    {
//                        using (StreamWriter sw = new StreamWriter(isfs))
//                        {
//                            foreach (ChannelInfo item in channelInfos)
//                            {
//                                data = item.VID + "|||||" + item.Title + "|||||" + item.PlayLink + "|||||" + item.ImgUrl + "|||||" + item.IsHD + "|||||" + item.IsRecommend + "|||||" + item.IsNew + "|||||" + item.Is3D + "|||||" + item.Bitrate + "|||||" + item.ResolutionWidth + "|||||" + item.ResolutionHeight + "|||||" + item.TypeID + "|||||" + item.SlotUrl;
//                                sw.WriteLine(data);
//                            }
//                            sw.Close();
//                        }
//                    }
//                }
//            }
//            catch (IsolatedStorageException exp)
//            {
//                // Log
//                LogManager.Ins.DebugLog("IsolatedStorageException: " + exp);
//                throw exp;
//            }
//            catch (Exception exp)
//            {
//                throw exp;
//            }
//        }

//        public List<ChannelInfo> GetFavours()
//        {
//            try
//            {
//                List<ChannelInfo> channelInfos = new List<ChannelInfo>();
//                string readPath = _filePath;
//                string content = string.Empty;
//                using (IsolatedStorageFile storeFile = IsolatedStorageFile.GetUserStoreForApplication())
//                {
//                    if (!storeFile.FileExists(readPath))
//                        return channelInfos;

//                    using (StreamReader sr = new StreamReader(storeFile.OpenFile(readPath, FileMode.Open, FileAccess.Read)))
//                    {
//                        while (true)
//                        {

//                            string line = sr.ReadLine();
//                            if (string.IsNullOrEmpty(line))
//                            {
//                                LogManager.Ins.DebugLog("内容为空跳出");
//                                break;
//                            }
//                            string[] strTemp = Utils.Splite(line, "|||||");
//                            if (strTemp.Length < 12)
//                            {
//                                // Log 小于6
//                                LogManager.Ins.DebugLog("strTemp.Length的个数为" + strTemp.Length);
//                                continue;
//                            }
//                            int channelVID;
//                            if (!int.TryParse(strTemp[0], out channelVID))
//                            {
//                                // Log
//                                LogManager.Ins.DebugLog("int.TryParse(strTemp[0], out channelVID)异常:" + int.TryParse(strTemp[0], out channelVID).ToString());
//                                continue;
//                            }
//                            string channelTitle = strTemp[1];
//                            string playLink = strTemp[2];
//                            string imgUrl = strTemp[3];
//                            bool isHD;
//                            if (!bool.TryParse(strTemp[4], out isHD))
//                            {
//                                //Log
//                                LogManager.Ins.DebugLog("isHD TryParse()异常");
//                                continue;
//                            }
//                            bool isRecommend;
//                            if (!bool.TryParse(strTemp[5], out isRecommend))
//                            {
//                                //Log
//                                LogManager.Ins.DebugLog("isRecommend TryParse()异常");
//                                continue;
//                            }
//                            bool isNew;
//                            if (!bool.TryParse(strTemp[6], out isNew))
//                            {
//                                //Log
//                                LogManager.Ins.DebugLog("isNew TryParse()异常");
//                                continue;
//                            }
//                            bool is3D;
//                            if (!bool.TryParse(strTemp[7], out is3D))
//                            {
//                                //Log
//                                LogManager.Ins.DebugLog("is3D TryParse()异常");
//                                continue;
//                            }
//                            int bitrate;
//                            if (!int.TryParse(strTemp[8], out bitrate))
//                            {
//                                // Log
//                                LogManager.Ins.DebugLog("int.TryParse(strTemp[8], out bitrate)异常:");
//                                continue;
//                            }
//                            int width;
//                            if (!int.TryParse(strTemp[9], out width))
//                            {
//                                // Log
//                                LogManager.Ins.DebugLog("int.TryParse(strTemp[9], out width)异常:");
//                                continue;
//                            }
//                            int height;
//                            if (!int.TryParse(strTemp[10], out height))
//                            {
//                                // Log
//                                LogManager.Ins.DebugLog("int.TryParse(strTemp[10], out height)异常:");
//                                continue;
//                            }
//                            int typeID;
//                            if (!int.TryParse(strTemp[11], out typeID))
//                            {
//                                LogManager.Ins.DebugLog("!int.TryParse(strTemp[11], out typeID)异常");
//                                continue;
//                            }
//                            string slotUrl = strTemp[12];

//                            ChannelInfo channelInfo = new ChannelInfo(channelVID, channelTitle, playLink, imgUrl, bitrate, isHD, isRecommend, isNew, is3D, width, height, typeID, slotUrl);
//                            channelInfos.Add(channelInfo);
//                        }   //end while
//                        return channelInfos;
//                    }   //end using
//                }   //end using
//            }
//            catch (Exception exp)
//            {
//                throw exp;
//            }
//        }

//        /// <summary>
//        /// 删除文件
//        /// </summary>
//        public void ClearFavourRecords()
//        {
//            using (IsolatedStorageFile storeFile = IsolatedStorageFile.GetUserStoreForApplication())
//            {
//                if (storeFile.FileExists(_filePath))
//                    storeFile.DeleteFile(_filePath);
//            }
//        }
//    }
//}
