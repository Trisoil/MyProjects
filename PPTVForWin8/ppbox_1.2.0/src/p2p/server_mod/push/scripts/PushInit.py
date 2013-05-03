#coding=utf-8
import threading
import httplib
import time
import datetime
import urllib
import urllib2
import codecs
import string
import StringIO
import re 
import sys
import xml
import os
import socket

import PushBase

class NewHot_Content:

    def __init__(self):
        self.push_base = PushBase.ContentBase()
        self.push_content = PushBase.PushContent(self.push_base)
        self.push_episode = PushBase.PushEpisodes(self.push_base, self.push_content)

    def DownloadAndUpdateContentList(self, todaystring, yesterdaystring):

        urltoday = PushBase.CONFIG.URL_DAC_TOP + todaystring + PushBase.CONFIG.URL_DAC_TOP_PARAM
        urlyesterday = PushBase.CONFIG.URL_DAC_TOP + yesterdaystring + PushBase.CONFIG.URL_DAC_TOP_PARAM

        print urltoday
        print urlyesterday
        yr_response = self.push_base.RetrieveURL(urlyesterday, 3, 10)
        today_response = self.push_base.RetrieveURL(urltoday, 3, 10)
        if (yr_response is None) or (today_response is None) :
            print "failed to retrieve hotcontent list from DAC, give up"
            return
        yr_data = yr_response.read()
        response_data = today_response.read()

        #DAC returns data in UTF-8 coding.
        yesterday_map = self.push_base.ParseContentList(yr_data)
        today_map = self.push_base.ParseContentList(response_data)
        for eachkey in today_map.keys():
            if eachkey not in yesterday_map:
                if today_map[eachkey].name in self.push_content.new_content_map :
                    oindex = self.push_content.new_content_map[today_map[eachkey].name].index
                    if (oindex < today_map[eachkey].index) :
                        print "updated old index is ", oindex, "new is ", today_map[eachkey].index 
                        self.push_content.new_content_map[today_map[eachkey].name] = today_map[eachkey] 
                    else:
                        print "ignored old index is ", oindex, "new is ", today_map[eachkey].index 
                else:
                    self.push_content.new_content_map[today_map[eachkey].name] = today_map[eachkey] 

    def Start(self, mtype):
        if (mtype == 1) :
          print "------------------------------------"
          print datetime.datetime.now(), " Download and update hot content from DAC..."
          #we are going to deal with hot_movies first.
          # TODO
          # self.push_content.Download_HotMovie_List()

          now = datetime.datetime.now()
          oneday = datetime.timedelta(-1)
          now = now + oneday 
          yesterd = now + oneday
          for i in range(PushBase.CONFIG.URL_DAC_INIT_LOOKBACK_RANGE):
              todaystring = now.strftime("%Y-%m-%d")
              yesterdaystring = yesterd.strftime("%Y-%m-%d")

              self.DownloadAndUpdateContentList(todaystring, yesterdaystring)
              now = now + oneday
              yesterd = now + oneday

          self.push_content.CreateContentXml()
          print datetime.datetime.now(), "...Done"
          print "------------------------------------"
        if (mtype == 2) :
          print "------------------------------------"
          print datetime.datetime.now(), "Download and process new uploaded content from EPG..."
          self.push_episode.DownloadNewUpload_Episodes(1)
          print datetime.datetime.now(), "...Done"
          print "------------------------------------"


class VIP_Content_Init:
    def __init__(self):
        self.vip_content = PushBase.VIP_Content()

    def Start(self):
        self.vip_content.DownloadAndUpdateVIPRIDs(1, "")

        #Now we need to Load all the RIDs and query their status
        print "-------------------------"
        print "Now we verifies the RIDs"
        self.vip_content.push_episode.LoadSavedEpisodeXML(self.vip_content.cached_rid_filelist, self.vip_content.cached_dir)
        new_added = self.vip_content.ReBuildRIDList(1)
        if new_added > 0:
            self.vip_content.WriteRIDListIntoDisk()
        else:
            print "No new RID added, skip serialization..."
        print "-------------------------"

        print "\nDone. Goodbye!"


if len(sys.argv) > 1 : 
    print "op mode is ", sys.argv[1]
    opmode = int(sys.argv[1])

    if opmode == 0 or opmode == 1:
        content_file = NewHot_Content()
        content_file.Start(1)
        content_file.Start(2)
    
    if opmode == 0 or opmode == 2:
        content_vip = VIP_Content_Init()
        content_vip.Start()
else:
    sys.exit("\nUsage: %s <mode>\n\tMode=0: All\n\tMode=1: Rebuild Hot Content/Episodes.\n\tMode=2: VIP Data Only\n" % sys.argv[0])

