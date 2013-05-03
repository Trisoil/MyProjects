#coding=utf-8
import threading
import time
import datetime
import sys
import os
import gc

import PushBase

class NewHot_Content:
    def __init__(self):
        self.push_base = PushBase.ContentBase()
        self.push_content = PushBase.PushContent(self.push_base)
        self.push_episode = PushBase.PushEpisodes(self.push_base, self.push_content)

    def DownloadAndUpdateContentList(self):
        #we are going to deal with hot_movies first.
        ##TODO 
        ##self.Download_HotMovie_List()

        now = datetime.datetime.now()
        oneday = datetime.timedelta(-1)
        hourstr = now.strftime("%H");

        yesterd = now + oneday 
        todaystring = now.strftime("%Y-%m-%d")
        yesterdaystring = yesterd.strftime("%Y-%m-%d")
        #todaystring = "2011-10-30"
        #yesterdaystring = "2011-10-29"
        #urltoday = URL_DAC_TOP + todaystring + URL_DAC_TOP_PARAM
        #urlyesterday = URL_DAC_TOP + yesterdaystring + URL_DAC_TOP_PARAM
        #print urltoday
        #print urlyesterday

        yr_data = ""
        response_data = ""
        try:
            fyr = open("hot-" + yesterdaystring + ".txt", "r")
            yr_data = fyr.read()
            fyr.close()
            ftd = open("hot-" + todaystring + ".txt", "r")
            response_data = ftd.read()
            ftd.close()
        except:
            print "read hot file failed"
            return
        
        #DAC returns data in UTF-8 coding.
        yesterday_map = self.push_base.ParseContentList(yr_data)
        today_map = self.push_base.ParseContentList(response_data)
        for eachkey in today_map.keys():
            if eachkey not in yesterday_map:
                # the whole map is in UNICODE
                self.push_content.new_content_map[eachkey] = today_map[eachkey] 


    def Start(self, mode):
        imode = int(mode)
        if ( (imode == 1) or (imode == 0) ) :
            print datetime.datetime.now(), " Download and update hot content from DAC..."
            self.DownloadAndUpdateContentList()
            self.push_content.CreateContentXml()
            print datetime.datetime.now(), "...Done"
        if ( (imode == 2) or (imode == 0) ):
            print datetime.datetime.now(), "Download and process new uploaded content from EPG..."
            self.push_episode.DownloadNewUpload_Episodes()
            print datetime.datetime.now(), "...Done"


class VIP_Content_Regular:
    def __init__(self):
        self.vip_content = PushBase.VIP_Content()

    def Start(self):
        latest_date = self.vip_content.LoadRIDListFromDiskCache()
        self.vip_content.DownloadAndUpdateVIPRIDs(0, latest_date)

        new_added = self.vip_content.ReBuildRIDList(0)

        #we do some memory cleaning
        self.vip_content.push_episode.ClearMemory()
        gc.collect()

        if new_added > 0:
            self.vip_content.WriteRIDListIntoDisk()
        else:
            print "No new RID added, skip serialization..."
    
        self.vip_content.QueryRIDCount()
        self.vip_content.GeneratePseudoVIPEpisode()
        print "\nDone. Goodbye!"

class NewHot_Tracking:
    def __init__(self):
        self.push_base = PushBase.ContentBase()
        self.push_content = PushBase.PushContent(self.push_base)
        self.push_rids = PushBase.Push_RIDs(self.push_base, self.push_content)

    def start(self):
        self.push_rids.QueryRIDCount()
        self.push_rids.GenerateRIDPushList()


if len(sys.argv) > 1 : 
    print "op mode is ", sys.argv[1]
    opmode = int(sys.argv[1])

    if opmode == 0 or opmode == 1 or opmode == 2:
        content_file = NewHot_Content()
        content_file.Start(sys.argv[1])

    startvip = 0
    if len(sys.argv) > 2:
        if int(sys.argv[2]) == 0:
            startvip = 1   
    if startvip==1 or opmode == 3:
        content_vip = VIP_Content_Regular()
        content_vip.Start()
        #content_vip.vip_content.TestCmdList()

    if opmode == 0 or opmode == 4:
        #start the content TRACKER count.
        newhot_track = NewHot_Tracking() 
        newhot_track.start()

else:
    sys.exit("\nUsage: %s <mode>\n\tMode=0: All (except VIP Data, use '%s <mode> 0' to force VIP data\n\tMode=1: DAC content.\n\tMode=2: EPG episodes\n\tMode=3: VIP Data Only\n" % (sys.argv[0], sys.argv[0]))


