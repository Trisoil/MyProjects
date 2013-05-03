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
import gzip
import re 
import sys
import os
import socket
import HTMLParser
import random    #random.shuffle(items)
from subprocess import Popen, list2cmdline, PIPE
import subprocess
from Queue import Queue

import xml.dom.minidom
import xml.parsers.expat
from xml.etree.ElementTree import ElementTree
from xml.etree.ElementTree import Element,SubElement,dump,Comment,tostring
from xml.etree import ElementTree as ET

import push_config as CONFIG

#config verfications 
print "**************************************"
print " Push Scripts V" + CONFIG.PUSH_VERSION
print "**************************************"
print "Verifying Config Data..."
if CONFIG.HOTLIST_DISK_PATH[-1] != os.sep:
    CONFIG.HOTLIST_DISK_PATH = CONFIG.HOTLIST_DISK_PATH + os.sep
    print "HOTLIST_DISK_PATH set to " + CONFIG.HOTLIST_DISK_PATH
if CONFIG.VIP_DATA_DIR_NAME[-1] != os.sep:
    CONFIG.VIP_DATA_DIR_NAME = "." + os.sep + CONFIG.VIP_DATA_DIR_NAME + os.sep
    print "VIP_DATA_DIR_NAME set to " + CONFIG.VIP_DATA_DIR_NAME 
print "**************************************"

#############################################
#############################################
# Content is the base class.
#############################################
class Content:
    def __init__(self, name, index):
        self.name = name
        self.index = index
        self.ctype = 'TV'

class Episode:
    def __init__(self):
        self.content_type = "" 
        self.content_name = ""
        self.content_index = ""
        self.episode_bitrate = ""
        self.episode_br_type = ""
        self.episode_id = "" 
        self.episode_name = ""
        self.episode_encode = ""
        self.rid_count = 0 
        self.rid_dict = {} 
        self.episode_online_time = "" 
        self.category1 = "Default"
        self.category2 = "Default"

    def reset(self, ctype, r_name, r_index, brate, br_type, ep_id, episode_name, ep_name, max_key, rid_dict, ep_online, category1 = "", category2 = ""):
        self.content_type = ctype
        self.content_name = r_name
        self.content_index = r_index
        self.episode_bitrate = brate
        self.episode_br_type = br_type
        self.episode_id = ep_id
        self.episode_name = episode_name
        self.episode_encode = urllib.quote(ep_name)
        self.rid_count = max_key
        self.rid_dict = rid_dict
        self.episode_online_time = ep_online
        if category1 != "":
            self.category1 = category1
        if category2 != "":
            self.category2 = category2

class RID:
    def __init__(self):
        self.rid_base = ""
        self.rid_copies = "0"
        self.rid_time = CONFIG.VIP_DATA_DEFALT_OLD_DATE_INT
        self.priority = "0" 
        self.fullstring = "" 
        self.content_id = ""
        self.encode_name = ""
        self.segindex = ""
        self.push_target = str(CONFIG.VIP_CONTROL_RID_TARGET)
        self.category = "Default"

    def reset(self, rid_base, rcount, timestamp, full_string, content_id, encode_name, segindex, category="", push_target = "0", priority = "-1"):
        self.rid_base = rid_base
        self.rid_copies = rcount
        if timestamp != "":
            self.rid_time = timestamp
        if priority != "-1":
            self.priority = priority
        self.fullstring = full_string 
        self.content_id = content_id
        self.encode_name = encode_name
        self.segindex = segindex
        if push_target != "0":
            self.push_target = push_target
        if category != "":
            self.category = category

    def reset_short(self, rid_base, rcount, timestamp):
        self.rid_base = rid_base
        self.rid_copies = rcount
        if timestamp != "":
            self.rid_time = timestamp


#############################################
#############################################
# For Parsing BAIDU top movie list:
#
# HTML_TableParser passes one HTML data into individual entries.
# MyHtmlParser deals with one entry in the HTML data.
#############################################
class MyHtmlParser(HTMLParser.HTMLParser):
    def __init__(self):
        HTMLParser.HTMLParser.__init__(self)
        self.html_data = ""

    def handle_data(self,data):
        self.html_data = data

    def get_data(self):
        #print self.html_data
        return self.html_data.decode('utf-8')

class HTML_TableParser:
    def __init__(self):
        self.tab_num = 0
        self.movie_list = []

    def parseTable(self, tdata):
        data = '<?xml version="1.0" encoding="utf-8"?>' + tdata
        dom = xml.dom.minidom.parseString(data)
        table = dom.getElementsByTagName('table')[0]
        tbody = table.getElementsByTagName('tbody')[0]
        trs = tbody.getElementsByTagName('tr')
        i = 0
        for tr in trs:
            td = tr.getElementsByTagName('td')[0]
            html_parser = MyHtmlParser()
            html_parser.feed(td.toxml())
            if (i != 0):  #we ignore the Table Description
                self.movie_list.append(html_parser.get_data())
            i += 1

        #print self.movie_list
        print "Total " + str(len(self.movie_list)) + " hot movies imported from BAIDU"
        return self.movie_list

class MPing:
    def __init__(self):
        self.matcher = re.compile("(\d+.\d+)/(\d+.\d+)/(\d+.\d+)/(\d+.\d+)")
        self.rtt_map = {}

    def pinger(self, threadid, q):
        while True:
            ip = q.get()
            #print "thread ", threadid, "Ping ", ip, CONFIG.PING_ARG1, CONFIG.PING_ARG2_NUM
            ping = subprocess.Popen(
                ["ping", str(CONFIG.PING_ARG1), str(CONFIG.PING_ARG2_NUM), ip],
                stdout = subprocess.PIPE, 
                stderr = subprocess.PIPE
            )
            out, error = ping.communicate()
            outarray = out.split()
            for item in outarray:
                result = self.matcher.match(item)
                if result:
                    rarray = result.groups()
                    if len(rarray) == 4:
                        self.rtt_map[ip] = float(rarray[1])

            q.task_done()

    def current_ping(self, ips):
        num_threads = CONFIG.PING_NUM_THREADS 
        queue = Queue()

        print "IPS are ", ips

        for i in range(num_threads):
            worker = threading.Thread(target=self.pinger, args=(i, queue))
            worker.setDaemon(True)
            worker.start()
        
        for ip in ips:
            queue.put(ip)
        queue.join()

        return self.rtt_map


#############################################
#############################################
# CONTENT BASE class:
#
# Deal with name parsing, URL retrieving, etc.
#############################################
class ContentBase:

    #############################
    #Set the system default coding to utf-8
    def __init__(self):
        self.inited = 1 
        default_encoding = 'utf-8'
        if sys.getdefaultencoding() != default_encoding:
            reload(sys)
            sys.setdefaultencoding(default_encoding)
        self.ridmatch_obj = re.compile('^rid=(?P<onerid>[0-9A-Z]+)\&filelength=(?P<fl>\d+)\&blocksize=(?P<bs>\d+)\&blocknum=(?P<bn>\d+)')

    def exec_commands(self, cmds):

        if not cmds: 
            print "empty list"
            return # empty list

        def done(p):
            return p.poll() is not None
        def success(p):
            return p.returncode == 0
        def fail(p):
            print p, "failed"

        max_task = CONFIG.VIP_CONTROL_MAX_EXT_PROCESS
        task_start_time = {} 
        if len(cmds) < max_task:
            max_task = len(cmds)
        processes = []
        loop_count = 0
        check_kill = 0
        current_epoch_time = 0
        while True:
            while cmds and len(processes) < max_task:
                task = cmds.pop()
                print list2cmdline(task)
                p = Popen(task)
                processes.append(p)
                task_start_time[p.pid] = self.GetEpochTime()

            if loop_count > 600:
                check_kill = 1  
                loop_count = 0
                current_epoch_time = self.GetEpochTime()
            for p in processes:
                pid = p.pid
                if done(p):
                    if success(p):
                        print p, "success"
                        processes.remove(p)
                    else:
                        print p, "failed"
                        processes.remove(p)
                else:
                    #we check the time used 
                    if check_kill == 1:
                        timesec = current_epoch_time - task_start_time[pid]
                        if timesec > CONFIG.VIP_EXT_TIMEOUT_SEC:
                            #p.kill()
                            try:
                                print "send signal to ", p
                                p.send_signal(9)
                            except:
                                print "send signal failed"
                        else:
                            print "time elapsed ", timesec
                    
            check_kill = 0
            if not processes and not cmds:
                break
            else:
                time.sleep(0.05)
                loop_count += 1

    def XMLPrettify(self, elem, mycodec):
        #Return a pretty-printed XML string for the Element.
        rough_string = ET.tostring(elem, mycodec)
        reparsed = xml.dom.minidom.parseString(rough_string)
        return reparsed.toprettyxml(indent="  ")

    def GetEpochTime(self):
        uepoch = datetime.datetime(1970, 1, 1, 0, 0, )
        delta = datetime.datetime.utcnow() - uepoch
        seconds = delta.seconds + delta.days * 86400
        return seconds

    #############################
    #Parse "data" from string to its content name and episode index.
    # for example, PrisonBreak(第 1集).mp4 == (PrisonBreak, 1)
    #              Dota-20120321-TestEpisode.mp4 == (Dota, 20120321).
    # if failed, returns ('', 0)
    def ParseNameAndIndex(self,data):
        # 第 u7B2C  集  \u96C6  （\uFF08
        #input data should be in UNICODE
        m = re.match(ur"^(?P<ename>.+)\(第(?P<eindex>\d+)集", data)
        if m:
            r_name = m.group('ename')
            i_string = m.group('eindex')
            return r_name, i_string
        m = re.match(ur"^(?P<ename>.+)（第(?P<eindex>\d+)集", data)
        if m:
            r_name = m.group('ename')
            i_string = m.group('eindex')
            return r_name, i_string
        m = re.match(r"^(?P<ename>.+)-(?P<eindex>\d{8})-", data)
        if m:
            r_name = m.group('ename')
            i_string = m.group('eindex')
            return r_name, i_string
        return '','0'

    #############################
    # Retrieve a URL, with retries and timeout value.
    def RetrieveURL(self, url, retries, ssec = 1):
        count = retries 
        while count > 0 :
            ecode = 0
            try:
                m_response = urllib2.urlopen(url, timeout=ssec)
                return m_response
            except urllib2.HTTPError, e:
                #print >> sys.stderr, "cannot retrieve http", url, " return error ", e.code 
                ecode = e.code
            except urllib2.URLError, e:
                print >> sys.stderr, "cannot retrieve Drag, return error ", e.args

            count -= 1
            if ecode != 404:
                time.sleep(1)
            else:
                break 
        print "cannot retrive retrieve ", url 
        return None 

    ##############################
    # remove empty lines from txt
    def removeEmptyLine(self, txt):
        ret=""
        for l in txt.split("\n"):
            if l.strip()!='':
                ret += l + "\n"
        return ret

    ##############################
    # convert xml from gbk to utf-8 encoding, and change the encoding
    # indicator too. 
    def gbk2utf8(self, xml):
        xml1 = xml.decode("gbk")
        xml = xml1.encode("utf8")
        return xml.replace('encoding="GBK"', 'encoding="utf-8"')

    ##############################
    # convert xml from gb2312 to utf-8 encoding, and change the encoding
    # indicator too. 
    def gb2312_utf8(self, xml):
        xml1 = xml.decode("gb2312")
        xml = xml1.encode("utf8")
        return xml.replace('encoding="GB2312"', 'encoding="utf-8"')

    ##############################
    # Use our DRAG service to lookup an episode into a list RIDs. 
    #  at the same time, keeps all the episode related information 
    #  associated with provided episode.
    def processDragToRIDs(self, ctype, episode_name, ep_id, br_type, r_name, r_index, brate, ep_online, ep_category1, ep_category2) :
        #DRAG requires the show name to be in GBK coding
        one_episode = Episode()
        print ep_id, br_type, r_name, r_index, episode_name 
        ep_name = episode_name.decode("utf8").encode("gbk");
        if (ep_name.rfind('.mp4') >= len(ep_name) - 4):
            #the ep_name already has .mp4 suffix.
            ep_name_full = ep_name
            # we will remove .mp4 from ep_name.
            ep_name = ep_name_full[:-4]
        else:
            ep_name_full = ep_name + '.mp4'
        reprStr = repr(ep_name_full).replace(r'\x', '%')
        dragurl = CONFIG.URL_DRAG_SERVER +reprStr[1:-1]+ CONFIG.URL_DRAG_SERVER_PARAM
        drag_response = self.RetrieveURL(dragurl, 3)
        if drag_response is None:
            return -1, one_episode
        try:
            drag_data = drag_response.read()
        except:
            print "error during reading drag data"
            return -1, one_episode

        encoding = drag_response.headers.get('content-encoding', None)
        if encoding == 'gzip' :
            drag_data = gzip.GzipFile(fileobj = StringIO.StringIO(drag_data)).read()
            #print "Warning: gzip happened!", episode_name
        #drag_data = self.removeEmptyLine(drag_data)
        drag_data = re.sub(r'<video name="[^"]+">', '<video>', drag_data)
        #DRAG returns data in GBK, we need to convert it to UTF-8
        drag_data = self.gbk2utf8(drag_data)
        drag_file = StringIO.StringIO()
        drag_file.write(drag_data)
        drag_file.seek(0)
        try:
            drag_xml = ET.parse(drag_file);
        except xml.parsers.expat.ExpatError:
            print "Parsing Error"
            return -1, one_episode
        except xml.etree.ElementTree.ParseError:
            print "Parsing Error"
            return -1, one_episode
        d_root = drag_xml.getroot()
        d_allfiles = d_root.getiterator("segment");
        rid_dict = {}
        max_key = -1 
        rid_count = 0
        for eachfile in d_allfiles:
            #read attributes from each file.
            d_id = eachfile.attrib['no']
            d_rid = eachfile.attrib['varid']
            m = self.ridmatch_obj.match(d_rid)
            if m:
                rid = d_rid
                rid_dict[d_id] = rid
                rid_count += 1
                if int(d_id) > max_key :
                    max_key = int(d_id)
        print "---%d RID loaded." %rid_count
        one_episode.reset(ctype, r_name, r_index, brate, br_type, ep_id, episode_name, ep_name, max_key, rid_dict, ep_online, ep_category1, ep_category2)

        return max_key, one_episode 
 
    ################################
    # Check whether the "data" is a movie based on provided movie list.
    #    a movie match uses strict matching from the beginning.
    def ParseMovieName(self,data,m_list):
        for eachmovie in m_list:
            #sindex = data.find(eachmovie, 0)
            if (True == data.startswith(eachmovie)):
                return eachmovie
        return ''

    ################################
    # Parse data to find all the mp4 files, 
    #  then use regular expression to generate a list of CONTENTs that 
    #  seems to be TV contents use ParseNameAndIndex.
    def ParseContentList(self, r_data):
        m_content_map = {}
        response_data_list = r_data.split('\t')
        for edata in response_data_list:
            if edata.find('mp4') != -1:
                #show name is in UTF-8 code, we need to convert to UNICODE for
                #    pattern matching.
                try:
                    edata = edata.decode('utf-8')
                    #returned strings are in UINCODE?
                    # if so, then the content_map is all in UNICODE
                    resource_name, index_string = self.ParseNameAndIndex(edata)
                    if index_string != '0':
                        new_content = Content(resource_name, index_string)
                        m_content_map[resource_name] = new_content
                        #print resource_name.encode('utf-8'), index_string
                except:
                    print "cannot decode name " + edata
        return m_content_map

    ######################################################
    # Distribute the RID_MAP into a number of files with list of RIDs. 
    # return the file list.
    def DistributeRIDIntoFiles(self, rid_map, filebase):
        print "------------------------"
        print "Splitting the %d RIDs into files"%(len(rid_map))
        print "------------------------"
        keys = rid_map.keys()
        file_count = 0
        file_rid_count = 0
        file_list = []
        for eachkey in keys:
            if (file_rid_count == 0):
                current_file_name = filebase + str(file_count) 
                print "writing into file " + current_file_name
                #we will remove the old .out file.
                try:
                    oldfilename = current_file_name + ".out"
                    print "removing previous result file " + oldfilename
                    os.remove(oldfilename)
                except:
                    print "cannot remove file " + oldfilename
                    
                ffhandle = open(current_file_name, 'w')
                file_count += 1
                file_list.append(current_file_name)
            if ffhandle != None:
                ffhandle.write(eachkey + "\n")
                file_rid_count += 1
                if file_rid_count >= CONFIG.VIP_CONTROL_MAX_RIDS_PER_FILE:
                    ffhandle.close()
                    file_rid_count = 0

        print "In total %d files written."%file_count
        return file_list

    ###############################################
    # Assumble the command to call externally to query TRACKERs for peer
    # count.
    def AssembleExternalCommandList(self, filelist):
        cmd_list = []
        for eachfile in filelist:
            outputfile = eachfile + ".out"
            arglist = [CONFIG.VIP_EXT_BINARY, CONFIG.VIP_EXT_ARG1_CMD,
                       eachfile, CONFIG.VIP_EXT_ARG3_TIMEOUT, 
                       outputfile, CONFIG.VIP_EXT_ARG4_QUIT]
            print arglist
            cmd_list.append(arglist)

        return cmd_list

    ##################################################
    # Load the TRACKER query result files into meomry 
    def LoadExternalCommandResultFile(self, rid_map, filelist):
        print "-------------------------------"
        current_lookup_time = str(self.GetEpochTime())
        print "currrent timestamp is ", current_lookup_time
        failed_file_list = []
        copies_stats = {}
        for eachfile in filelist:
            outputfile = eachfile + ".out"
            try:
                query_count = 0
                ffhandle = open(outputfile, 'r')
                for line in ffhandle.readlines():
                    words = line.split()
                    if (len(words)>=2):
                        rid_str = words[0]
                        if rid_str in rid_map:
                            one_rid = rid_map[rid_str] 
                            one_rid.rid_copies = words[1]
                            one_rid.rid_time = current_lookup_time
                            rid_map[rid_str] = one_rid 
                            query_count += 1
                            if words[1] in copies_stats:
                                copies_stats[words[1]] = copies_stats[words[1]]+1
                            else:
                                copies_stats[words[1]] = 1
                ffhandle.close()
                print "%s produced %d queries."%(outputfile, query_count)
            except:
                failed_file_list.append(eachfile) 
        print copies_stats
        print "failed file list ", failed_file_list
        return current_lookup_time


#############################################
#############################################
# For HOT CONTENT: content is defined the top level unit of categories of
# epsisodes.
#############################################
class PushContent:
    def __init__(self, push_base):
        self.current_movie_list = []
        self.current_tvcontent_map = {}
        self.new_content_map = {}
        self.new_movie_list = []
        self.hotlist_disk_path = CONFIG.HOTLIST_DISK_PATH
        self.push_base = push_base
        self.current_list_retrieved = 0
        self.current_rid_retrieved = 0
        self.current_rid_map = {} 

    ##############################
    # Serialize the CONTENT list into XML. 
    def CreateContentXml(self):
        content_document = ElementTree()
        content_list = Element('contentlist')
        content_document._setroot(content_list)
        for content in self.new_content_map.values():
            #print content
            item = Element("content")
            content_list.append(item)
            # encode the name to UTF8
            #SubElement(item, 'name').text = content.name.encode('GB2312')
            #SubElement(item, 'name').text = content.name.decode('utf-8')
            SubElement(item, 'name').text = content.name
            SubElement(item, 'index').text = content.index
            SubElement(item, 'type').text = content.ctype
            #print content.name
            #print content.index
        for movie in self.new_movie_list:
            #print movie
            item = Element("content")
            content_list.append(item)
            #SubElement(item, 'name').text = movie.encode('GB2312')
            SubElement(item, 'name').text = movie.decode('utf-8')
            SubElement(item, 'index').text = "0"
            SubElement(item, 'type').text = "Movie"

        content_document.write('newhot_content_list.xml', 'utf-8')

    ##############################
    # Read the CURRENT pushing contents from the existing database.
    #   usually reads from the local disk, the file repositories of running
    #   PUSH server, that is, push_env_0/pushhot.clist.config.
    # This is for the purpose of pre-filtering, to save the number of queries
    #   to DRAG server: we only convert the episodes that match certain rules
    #   (that is, the current and new list of hot contents). 
    # If the reading fails, then a flag is set, and we have to convert
    #   a lot of new episodes into RIDs.
    def RetrieveCurrentContentList(self):
        #read the hotlist from disk.
        print "start to retrieve hot content list from local disk"
        self.current_movie_list = []
        self.current_tvcontent_map = {}
        self.current_list_retrieved = 0

        try:
            hconfig = open(self.hotlist_disk_path + CONFIG.HOTLIST_CONFIGFILE)
            hxml = hconfig.read()
            #config_data = self.push_base.gb2312_utf8(hxml)
            config_data = hxml
            config_file = StringIO.StringIO()
            config_file.write(config_data)
            config_file.seek(0)

            hc_xml = ET.parse(config_file)
            d_root = hc_xml.getroot()
            # for python 2.7, try d_root.iter()
            d_alli = d_root.getiterator("content")
            for eachfile in d_alli:
                element_name = eachfile.find('name')
                element_type = eachfile.find('type')
                show_name = element_name.text
                if (element_type == None):
                    show_type = "TV"
                else:
                    show_type = element_type.text
                #we store the name as UNICODE
                show_name.decode('utf-8')
                if (show_type == "TV"):
                    self.current_tvcontent_map[show_name] = show_type
                elif (show_type == "Movie"):
                    self.current_movie_list.append(show_name)
            print "TVList ", len(self.current_tvcontent_map)
            print "MovieList ", len(self.current_movie_list)
            self.current_list_retrieved = 1
        except:
            print >> sys.stderr, "Error during hquery handling"

    def RetrieveCurrentRIDList(self):
        #read the hotlist from disk.
        print "start to retrieve pushhot RID list from local disk"
        self.current_rid_map = {} 
        self.current_rid_retrieved = 0
        try:
            hconfig = open(self.hotlist_disk_path + CONFIG.HOTLIST_CONFIGFILE)
            hxml = hconfig.read()
            config_data = hxml
            config_file = StringIO.StringIO()
            config_file.write(config_data)
            config_file.seek(0)

            hc_xml = ET.parse(config_file)
            d_root = hc_xml.getroot()
            # for python 2.7, try d_root.iter()
            d_alli = d_root.getiterator("content")
            for eachfile in d_alli:
                element_type = eachfile.find('type')
                if (element_type == None):
                    show_type = "TV"
                else:
                    show_type = element_type.text
                ep_alli = eachfile.getiterator("episode")
                for eachep in ep_alli:
                    rid_alli = eachep.getiterator("RID")
                    for eachrid in rid_alli:
                        #Now we have the RIDS.
                        rid_string = eachrid.text
                        m = self.push_base.ridmatch_obj.match(rid_string)
                        if m:
                            ridshort = m.group('onerid')
                            one_rid = RID()
                            one_rid.reset_short(ridshort, "0", "")
                            self.current_rid_map[ridshort] = one_rid 

            print "current RIDList ", len(self.current_rid_map)
            self.current_rid_retrieved = 1
        except:
            print >> sys.stderr, "Error during hquery handling"

    ###############################
    # Download the top Movie list from Baidu RSS. 
    #   parse the content, and get the HTML table structure inside.
    #   then use HTML parser to get the list of movie names.
    def Download_HotMovie_List(self):
        print "-------------------------------------------"
        print "Start to retrieve TOP MOVIE list from baidu"
        movieurl = CONFIG.URL_BAIDU_HOTMOVIE
        print "RETRIEVE " + movieurl

        movie_response = self.push_base.RetrieveURL(movieurl, 3, 3)
        if movie_response is None:
            return -1
        movie_data = movie_response.read()
        encoding = movie_response.headers.get('content-encoding', None)
        if encoding == 'gzip' :
            movie_data = gzip.GzipFile(fileobj = StringIO.StringIO(movie_data)).read()
            #print "Warning: gzip happened!", episode_name
        movie_data = self.push_base.removeEmptyLine(movie_data)
        #DRAG returns data in GBK, we need to convert it to UTF-8
        movie_data = self.push_base.gbk2utf8(movie_data)
        # We will strip the part outside of <table></table>
        content_start = movie_data.find('<table>')
        content_end   = movie_data.rfind('</table>')
        if (content_start!=-1) and (content_end != -1):
            content_end = content_end + len('</table>')
            htmldata = movie_data[content_start:content_end]
            #we will remove all href.
            htmldata = re.sub('href=\".*\"', '', htmldata)
            print "Found entries" 
            tparser = HTML_TableParser()
            self.new_movie_list = tparser.parseTable(htmldata)
        else:
            print "Could find key 'table' in RSS format. Baidu upgraded?"

#############################################
#############################################
# For HOT UPLOAD EPISODES: 
#   parse the new ONLINE episodes from EPG, match them against the hot list, 
#   then produce the list of episodes with RIDS into the XML file. 
#############################################
class PushEpisodes:
    def __init__(self, push_base, push_content):
        self.new_episode_list = []
        self.load_episode_list = []
        self.push_base = push_base
        self.push_content = push_content
        self.br_type_stat = {}
        self.exclude_list = {}

    def ClearMemory(self):
        self.new_episode_list = []
        self.load_episode_list = []
        self.br_type_stat = {}
        self.exclude_list = {}

    ###########################################
    # Exclude functionality: we can provide a simple text file
    #   to exclude content with these patterns to be included.
    def LoadExcludeList(self, filename):
        print "---------------------------"
        print "Load Exclude List file " + filename 
        print "---------------------------"
        self.exclude_list = {}
        try:
            efile = open(filename, "r")
            for line in efile.readlines():
               linename = line.rstrip(' \r\n')
               #we use UNICODE internally
               linename.decode("utf-8")
               self.exclude_list[linename] = 1 
            efile.close()
        except: 
            print "Failed to load Exclude List file"

    ##################################################
    # Retrieve New Episodes from EPG, filter them against the 
    #  current TV/Movie list, then query the DRAG to generate 
    #  the valid episode/rid file.
    def DownloadNewUpload_Episodes(self, inited = 0):
        self.LoadExcludeList("exclude_content.txt")        

        self.new_episode_list = [] 
        if (inited == 0):
            self.push_content.RetrieveCurrentContentList()
        else:
            #for init script, no need to retrieve current list.
            self.push_content.current_list_retrieved = 1
        #merge lists
        cmovie_list = self.push_content.current_movie_list
        for item in self.push_content.new_movie_list:
           cmovie_list.append(item)
        #combined map is in UNICODE 
        ctvcontent_map = dict(self.push_content.current_tvcontent_map.items() + self.push_content.new_content_map.items())
        if (inited == 1):
            ep_url = CONFIG.URL_EPG_INIIT
        else:
            ep_url = CONFIG.URL_EPG_NEWUPLOAD
        print "---------------------------"
        print "RETRIEVE URL: " + ep_url
        print "---------------------------"
        episodes_response = self.push_base.RetrieveURL(ep_url, 3, 30)
        if episodes_response is None:
            return 
        episodes_data = episodes_response.read()
        # from EPG, data are actually in UNICODE format
        episodes_data = self.push_base.removeEmptyLine(episodes_data)
        episodes_file = StringIO.StringIO()
        episodes_file.write(episodes_data)
        episodes_file.seek(0)
        episodes_xml = ET.parse(episodes_file);
        e_root = episodes_xml.getroot()
        e_allfiles = e_root.getiterator("File");
        for eachfile in e_allfiles:
            #read attributes from each file.
            ep_name = eachfile.attrib['nm']
            ep_isvip = eachfile.attrib['vip']
            #we first parse it as TV content
            resource_name, index_string = self.push_base.ParseNameAndIndex(ep_name)
            if (ep_isvip == '1') : 
                #now we will have to iterate through all the bitrate.
                resource_name = "VIP"
                maxrate = self.ProcessNewEpisodeAllFiles('VIP', eachfile, ep_name, '0')
            #there may be duplications between passive pushing and VIP.
            if resource_name in self.exclude_list.keys():
                print "ignored Episode with name " + resource_name
                continue 
            if (index_string != '0') : 
                #this is TV like content
                #print ep_name, ep_id, resource_name, index_string,"\n"
                if (resource_name in ctvcontent_map) or (self.push_content.current_list_retrieved == 0):
                    maxrate = self.ProcessNewEpisodeAllFiles('TV', eachfile, resource_name, index_string)

            else:
                #this one does not follow TV like pattern, we will use movie
                # keyword to pass this.
                resource_name = self.push_base.ParseMovieName(ep_name, cmovie_list)
                if resource_name != '':
                    maxrate = self.ProcessNewEpisodeAllFiles('Movie', eachfile, resource_name, '0')

        self.CreateNewEpisodeXML()

    ########################################################
    # For One EPG 'File' XML Entry from EPG, process all the bitrate (ftfile)
    #  -- query DRAG, and put the result in SELF.new_episode_list.
    #  for one 'file' structure, we process all the ftfile files and the 
    #     file provided in the FILE element.
    def ProcessNewEpisodeAllFiles(self, ctype, eachfile, rsrc_name, rsrc_index):
        #we will have to iterate through all the bitrate.
        ep_filename = eachfile.attrib['fileName']
        ep_id = eachfile.attrib['id']
        ep_src_br = eachfile.attrib['br']
        ep_online = eachfile.attrib['cT']
        #TODO load the right category when EPG is ready
        try:
            ep_category1 = eachfile.attrib['cata1']
        except:
            ep_category1 = "Default"
        try:
            ep_category2 = eachfile.attrib['cata2']
        except:
            ep_category2 = "Default"
        #do we need to convert ep_online from normal time to epoch time.

        e_allbitrate = eachfile.getiterator("ftfile")
        count = 0
        maxrate = 0
        ep_filename_processed = 0
        max_ft = 0
        for eachbr in e_allbitrate:
            br_epname = eachbr.attrib['fileName']
            br_br = eachbr.attrib['bitrate']
            br_type = eachbr.attrib['ft']
            #br_type 5+ is for mobile.
            if br_type != '' and int(br_type) < 5:
                if max_ft < int(br_type):
                    max_ft = int(br_type)
            (maxid, one_episode) = self.push_base.processDragToRIDs(ctype, br_epname, ep_id, br_type, rsrc_name, rsrc_index, br_br, ep_online, ep_category1, ep_category2)
            self.new_episode_list = self.RecordEpisode(self.new_episode_list, maxid, one_episode)

            count += 1
            if (br_br > maxrate):
                maxrate = br_br
            if br_epname == ep_filename:
                ep_filename_processed = 1

        if ep_filename_processed == 0 and ep_filename != '':
            #here we are processing the top NM line, which is the source file.
            # its ft type is not available, we assume it is one level above
            # the max ft provided in the various bitrate streams.
            new_br_type = max_ft + 1
            (maxid, one_episode) = self.push_base.processDragToRIDs(ctype, ep_filename, ep_id, str(new_br_type), rsrc_name, rsrc_index, ep_src_br, ep_online, ep_category1, ep_category2)
            self.new_episode_list = self.RecordEpisode(self.new_episode_list, maxid, one_episode)

        return maxrate

    ###########################################################
    # Add one Episode into the provided list, and collect the bitrate
    #    distribution information.
    def RecordEpisode(self, episode_list, rid_count, tuples):
        if (rid_count != -1):
            episode_list.append(tuples)
            #we count the br_type here.
            mybr_type = tuples.episode_br_type
            if mybr_type in self.br_type_stat :
                self.br_type_stat[mybr_type] = self.br_type_stat[mybr_type] + 1
            else:
                self.br_type_stat[mybr_type] = 1
        return episode_list

    ###########################################################
    # Load the saved Episode/RID information into memory 
    #  memory location: SELF.loaded_episode_list
    def LoadSavedEpisodeXML(self, filelist, dir_name):
        #print the summary here.
        print "---------------------------------------"
        print " Load saved VIP Episode and RID list..."
        print "---------------------------------------"
        total_ep = 0
        total_rid = 0
        self.br_type_stat = {}
        ep_rid_stat = {}
        for eachfile in filelist:
            filename = dir_name + eachfile
            print "Loading " + filename
            hconfig = open(filename, 'r')
            rid_xml = ET.parse(hconfig)
            d_root = rid_xml.getroot()
            d_alli = d_root.getiterator('episode')
            for ep in d_alli:
                one_episode = Episode()
                one_episode.content_name = ep.find('show_name').text
                one_episode.content_index = ep.find('show_index').text 
                one_episode.content_type = ep.find('show_type').text
                one_episode.episode_name = ep.find('episode_name').text
                one_episode.episode_encode = ep.find('encode_name').text
                one_episode.episode_id = ep.find('episode_id').text
                one_episode.episode_bitrate = ep.find('episode_bitrate').text
                one_episode.episode_br_type = ep.find('bitrate_type').text
                one_episode.episode_online_time = ep.find('online_time').text
                one_episode.episode_online_time = ep.find('online_time').text
                one_episode.category1 = ep.find('cat1').text
                one_episode.category2 = ep.find('cat2').text
                element_RIDS = ep.find('RIDs')
                rid_alli = element_RIDS.getiterator('RID')
                one_episode.rid_dict = {}
                rid_count = 0
                for each_rid in rid_alli:
                    one_episode.rid_dict[str(rid_count)] = each_rid.text
                    rid_count += 1
                    total_rid += 1
                one_episode.rid_count = rid_count
                if rid_count in ep_rid_stat:
                    ep_rid_stat[rid_count] = ep_rid_stat[rid_count] + 1
                else:
                    ep_rid_stat[rid_count] = 1

                if rid_count == 0:
                    print "Ignored Episode without RIDs " + one_episode.episode_id 
                    continue;
                total_ep += 1
                self.RecordEpisode(self.load_episode_list, rid_count, one_episode)
        print "Total %d episode loaded with %d RIDS" %(total_ep, total_rid)
        for eachkey in self.br_type_stat:
            print "br_type " + str(eachkey) + " = " + str(self.br_type_stat[eachkey])
        for eachkey in ep_rid_stat:
            print "RID-PerEP["+ str(eachkey) + "] = " + str(ep_rid_stat[eachkey])

    #############################################################
    #  Write the SELF.new_episode_list into disk in XML format.
    def CreateNewEpisodeXML(self, vip_date = "", vip_dir = "") :
        #print the summary here.
        print "***********************"
        print "New Episode Summary:"
        print "***********************"
        for eachkey in self.br_type_stat:
            print "br_type " + str(eachkey) + " = " + str(self.br_type_stat[eachkey])
        print "Write Episode Data with RIDs into Disk..."
        ep_document = ElementTree()
        ep_list = Element('episodes')
        ep_document._setroot(ep_list)
        for ep_one in self.new_episode_list:
            item = Element('episode')
            ep_list.append(item)
            SubElement(item, 'show_name').text = ep_one.content_name
            SubElement(item, 'show_index').text = ep_one.content_index 
            if ep_one.content_type == 'VIP':
                SubElement(item, 'show_type').text = "VIP"
            elif ep_one.content_type == 'Movie':
                SubElement(item, 'show_type').text = "Movie"
            else:
                SubElement(item, 'show_type').text = "TV"
            SubElement(item, 'episode_name').text = ep_one.episode_name
            SubElement(item, 'encode_name').text = ep_one.episode_encode
            SubElement(item, 'episode_id').text = ep_one.episode_id
            SubElement(item, 'episode_bitrate').text = ep_one.episode_bitrate
            SubElement(item, 'bitrate_type').text = ep_one.episode_br_type 
            SubElement(item, 'online_time').text = ep_one.episode_online_time 
            SubElement(item, 'cat1').text = ep_one.category1
            SubElement(item, 'cat2').text = ep_one.category2
            #print ep_id, ep_name, encode_name
            rid_item = Element('RIDs')
            item.append(rid_item)
            for ein in range(ep_one.rid_count +1):
                if str(ein) in ep_one.rid_dict:
                    SubElement(rid_item, 'RID').text = ep_one.rid_dict[str(ein)]
                else:
                    SubElement(rid_item, 'RID').text = 'NULL'

        if vip_date == "":
            ep_document.write('newupload_content_list.xml', 'utf-8')
            return 'newupload_content_list.xml' 
        else:
            newfilename = CONFIG.VIP_DATA_RID_CACHE_NAME + "." + vip_date + ".xml"
            ep_document.write(vip_dir + newfilename, 'utf-8')
            return newfilename

class Push_RIDs:
    def __init__(self, push_base, push_content):
        self.push_base = push_base
        self.push_content = push_content
        self.current_lookup_time = CONFIG.VIP_DATA_DEFALT_OLD_DATE

    ###########################################
    # PUBLIC: get the peer count number for the RID_MAP 
    def QueryRIDCount(self):
        self.push_content.RetrieveCurrentRIDList()
        filelist = self.push_base.DistributeRIDIntoFiles(self.push_content.current_rid_map, CONFIG.PUSHHOT_RID_EXT_FILE_BASE)
        cmdlist = self.push_base.AssembleExternalCommandList(filelist)
        if len(cmdlist) <= 0:
            print "ERROR: NO external QUERY command formed, give up"
            return -1
        self.push_base.exec_commands(cmdlist)

        self.current_lookup_time = self.push_base.LoadExternalCommandResultFile(self.push_content.current_rid_map, filelist)

        if CONFIG.PUSHHOT_RID_QUERY_DATA_BACKUP == "True":
            #we will backup the data here. 
            timenow = datetime.datetime.now()
            timestring = timenow.strftime("%Y%m%d-%H-%M")
            tarname = CONFIG.push_log_path + '/phot-' + timestring + '.tgz'
            srcname = CONFIG.PUSHHOT_RID_EXT_FILE_BASE + '*.out'

            command = "tar cfz " + tarname + " " + srcname
            print command
            os.system(command)

    ###########################################
    # PUBLIC: generate the RID task file for PUSH server.
    def GenerateRIDPushList(self):
        #Now we have how many files. 
        print "-------------------------------"
        print "Generate RID Push List"
        print "-------------------------------"
        rid_document = ElementTree()
        rid_el_list = Element('hotrid_list')
        #SubElement(rid_el_list, 'time').text = self.current_maxdate 
        rid_document._setroot(rid_el_list)
        rid_se_count = 0
        rid_filtered_count = 0
        for (rkey, rvalue) in self.push_content.current_rid_map.items():
            #FILTER: we have several conditions to ignore the RID.
            filtered = self.FilterPushHotRIDs(rvalue)
            if filtered == 1:
                rid_filtered_count += 1
                continue
            item = Element('RID')
            rid_el_list.append(item)
            SubElement(item, 'id').text = rkey
            #SubElement(item, 'count').text = rvalue.rid_copies
            #SubElement(item, 'time').text = rvalue.rid_time
            if filtered == 2:
                SubElement(item, 'target').text = "0"
            else:
                SubElement(item, 'target').text = str(CONFIG.PUSHHOT_RID_QUERY_TARGET)
            rid_se_count += 1
        rid_document.write(CONFIG.PUSHHOT_RID_QUERY_OUTPUT_FILE, 'utf-8')
        print "Summary: pushhot %d rid written into %s" %(rid_se_count, CONFIG.PUSHHOT_RID_QUERY_OUTPUT_FILE) 
        print "Summary: pushhot %d rid filtered." %rid_filtered_count
        self.push_content.current_rid_map = {}


    def FilterPushHotRIDs(self, rvalue):
        rid_tracker_count = int(rvalue.rid_copies)
        #print rid_tracker_count,  rvalue.rid_time, self.current_lookup_time
        if rvalue.rid_time != self.current_lookup_time: 
            return 1 
        if rid_tracker_count > CONFIG.PUSHHOT_RID_QUERY_PEER_HIGH:
            return 2 
        if rid_tracker_count > CONFIG.PUSHHOT_RID_QUERY_PEER_LOW:
            return 1 
        return 0 


#############################################
#############################################
# VIP All Content Pushing Class
#
#  Query all the VIP RIDs and stores them. 
#  Query tracker for RID's p2p peer copies, and produced a RID list
#   with low peer count, such that PUSH can push them
# DATA PATH in INIT mode:
#    EPG-VIP data => cached EPG VIP raw data file
#     => EPG/RIDS files/memory => RID_MAP => stored RIDMAP
# DATA PATH in UPDATE mode:
#    stored RIDMAP => RID_MAP -------------|
#    new EPG-VIP data => EPG/RIDs memory-------=> RIDMAP => RID with peer# 
#    => RID task file for PUSH to load
#############################################
class VIP_Content:
    def __init__(self):
        print "******************************"
        print "Start to Process VIP DATA..."
        print "******************************"
        self.push_base = ContentBase()
        self.push_content = PushContent(self.push_base)
        self.push_episode = PushEpisodes(self.push_base, self.push_content)

        self.RID_MAP = {}
        self.current_maxdate = ""
        self.current_lookup_time = CONFIG.VIP_DATA_DEFALT_OLD_DATE
        self.cached_dir = CONFIG.VIP_DATA_DIR_NAME
        self.cached_rid_filelist = []
        self.cached_rawdata_filelist = []
        self.reobj_rid = re.compile('%s.+(?P<date_end>\d{8})\.xml'%CONFIG.VIP_DATA_RID_CACHE_NAME)
        self.reobj_rawdata = re.compile('%s.+(?P<date_start>\d{8})\.(?P<date_end>\d{8})\.xml'%CONFIG.VIP_DATA_NAME_PREFIX)
        self.vip_exclude_list = {}
        self.vip_include_list = {}

    ###################################################
    # Read the saved 'Multiple' EPG VIP rawdata (in XML) and 
    #   load them into memory (SELF.push_episode.new_episode_list)
    # Write the aggregated re-orged (with RIDs) episodes data into file. 
    # Keep all the writed file names in SELF.cached_rid_filelist 
    def RebuildVipEpisodeCacheFromRawData(self, filelist):
        #read files from local disk.
        mindate = '99999999'        
        maxdate = '00000000'        
        for fname in filelist:
            #has to be XXXXXXXX.XXXXXXXX.xml format
            m = self.reobj_rawdata.match(fname)
            if m:
                print "processing " + fname 
                date_start = m.group('date_start')
                date_end = m.group('date_end')
                self.ProcessCachedVipEgpRawData(fname)
                if date_start < mindate:
                    mindate = date_start
                if date_end > maxdate:
                    maxdate = date_end
            else:
                print "error: " + fname + " file name format problem"

        if maxdate != '00000000':
            wfilename = self.push_episode.CreateNewEpisodeXML(maxdate, self.cached_dir)
            self.cached_rid_filelist.append(wfilename)
       
        print "New maxdate is ", maxdate
        return maxdate

    #####################################################
    # Read and Parse 'ONE' EPG VIP raw data file.
    def ProcessCachedVipEgpRawData(self, filename):
        hfile = open(self.cached_dir + filename)
        hxml = hfile.read()
        #config_data = self.push_base.gb2312_utf8(hxml)
        config_data = hxml
        config_file = StringIO.StringIO()
        config_file.write(config_data)
        config_file.seek(0)
        self.ParseEpgEpisodeXML(config_file)

    #####################################################
    # Parse a read handle that contains 'ONE' EPG VIP raw data file, 
    #   query the DRAG, and save the parsing result in
    #   SELF.push_episode.new_episode_list
    def ParseEpgEpisodeXML(self, config_file):
        if config_file:
            episodes_xml = ET.parse(config_file);
            e_root = episodes_xml.getroot()
            e_allfiles = e_root.getiterator("File");
            for eachfile in e_allfiles:
                ep_name = eachfile.attrib['nm']
                #resource_name, index_string = self.push_base.ParseNameAndIndex(ep_name)
                maxrate = self.push_episode.ProcessNewEpisodeAllFiles('VIP', eachfile, ep_name, '0')
        
    ###################################################
    # Read the File List from the vip cached directory, 
    #  1) to find out all the cached EPG VIP rawdata files.
    #  2) to parse the file name to figure out the most rencent cache date. 
    # the filelist in stored in SELF.cached_rawdata_filelist
    def DetectEPGQueryDateRangeBasedOnEPGCache(self):
        dirList = os.listdir(self.cached_dir)
        cached_rid_maxdate = CONFIG.VIP_DATA_DEFALT_OLD_DATE 
        for fname in dirList:
            #has to be XXXXXXXX.XXXXXXXX.xml format
            m = self.reobj_rid.match(fname)
            if m:
                print "analyze " + fname 
                date_end = m.group('date_end')
                if date_end > cached_rid_maxdate:
                    cached_rid_maxdate = date_end
                self.cached_rid_filelist.append(fname) 
        for fname in dirList:
            m = self.reobj_rawdata.match(fname)
            if m:
                print "analyze " + fname 
                date_start = m.group('date_start')
                date_end = m.group('date_end')
                if date_end <= cached_rid_maxdate:
                    print "skipped " + fname + "as it is already processed"
                else:
                    self.cached_rawdata_filelist.append(fname)
            else:
                print "ignored file " + fname + " due to file name format problem"
        print "Detect raw data max date is ",  cached_rid_maxdate 
        return cached_rid_maxdate

    ###################################################
    # PUBLIC ENTRY POINT: Generate the VIP RID list. 
    #  there are two modes: INIT, UPDATE
    # INIT mode: 1) lookup current EPG raw data cache, load the cached data
    #            2) query the EPG API and write the newest cached data into file.
    #            3) convert EPG raw data into Episode/RID files, store them.
    #    in init mode, the start_date argument is ignored.
    #    OUTPUT: SELF.push_episode.new_episode_list, Episode/RID cache files.
    # UPDATE mode: 1) read the start_date.
    #            2) query the EPG API and read the update EPG data into memory
    #    OUTPUT: SELF.push_episode.new_episode_list
    def DownloadAndUpdateVIPRIDs(self, is_init, start_date):
        if is_init == 1:
            print "-------------------------------"
            print "***Init Mode..."
            print " Load Save Raw EPG Data...."
            print "-------------------------------"
            #for init case, start_date passing in will be ignored
            known_maxdate = self.DetectEPGQueryDateRangeBasedOnEPGCache()
            if len(self.cached_rawdata_filelist) > 0: 
                new_max_date = self.RebuildVipEpisodeCacheFromRawData(self.cached_rawdata_filelist)
                if new_max_date > known_maxdate:
                    known_maxdate = new_max_date
            else:
                print "No unprocesed VIP RAW DATA file"
        else:
            print "-------------------------------"
            print "***Update Mode..."
            print "current RID_MAP size = ", len(self.RID_MAP)
            print "-------------------------------"
            known_maxdate = start_date
        self.current_maxdate = known_maxdate 
        #get current date
        now = datetime.datetime.now()
        todaystring = now.strftime("%Y%m%d")
        if todaystring > known_maxdate :
            #assemble the string, and write the content to disk.
            new_url = CONFIG.URL_EPG_VIP_BASE + known_maxdate + "&end=" + todaystring
            print "RETRIEVE VIP URL " + new_url
            response_data = self.push_base.RetrieveURL(new_url, 2, 5)
            if is_init == 1:
                print "Write the query data into disk..."
                new_filename = CONFIG.VIP_DATA_NAME_PREFIX + "." + known_maxdate + "." + todaystring + ".xml"
                new_filehandle = open(self.cached_dir + new_filename, 'w')
                new_filehandle.write(response_data.read())
                new_filehandle.close()
                self.cached_rawdata_filelist = []
                self.cached_rawdata_filelist.append(new_filename) 
                self.RebuildVipEpisodeCacheFromRawData(self.cached_rawdata_filelist)
            else:
                #if not init case, 
                print "Process the query data in memory..."
                self.ParseEpgEpisodeXML(response_data)

            self.current_maxdate = todaystring
        else:
            print "cached date is current, no need to query EPG..."

    ###################################################
    # PUBLIC ENTRY POINT: Generate the VIP RID list. 
    #  Assume we either have SELF.cached_rid_filelist (EPG/RIDS files), such
    #            that we can load them into SELF.loaded_episode_list
    #       or we have SELF.new_episode_list already. 
    #  Should be called after DownloadAndUpdateVIPRIDs(),
    # DATA SOURCE:
    #  there are two modes: INIT, UPDATE
    #     INIT mode: from cached EPG/RIDS files into loaded EPG/RIDs.
    #     UPDATE mode: loaded RID_MAP and new EPG/RIDS file in memory  
    # OUTPUT: SELF.RID_MAP
    def ReBuildRIDList(self, is_init):
        #FILTER: load the config
        self.LoadVIPFilterConfig()

        if is_init == 1:
            print "***Init Mode..."
            if len(self.push_episode.load_episode_list) <= 1:
                self.push_episode.LoadSavedEpisodeXML(self.cached_rid_filelist, self.cached_dir)
        else:
            print "***Update Mode..."
            self.push_episode.load_episode_list = self.push_episode.new_episode_list 
        #How do we build the RID list cache.
        print "current RID_MAP size = ", len(self.RID_MAP)
        print "loaded episode list include " + str(len(self.push_episode.load_episode_list)) + " items."
        rid_dup = 0
        rid_error = 0 
        rid_sum = 0
        rid_added = 0
        filter_ep_count = 0
        for eachep in self.push_episode.load_episode_list:
            #FILTER: we do filtering during EP/RID list to vip_rid_store conversion.
            filtered = self.FilterVIPEpisode(eachep)
            if filtered == 1:
                filter_ep_count += 1
                continue
            rid_category = eachep.category1 + '.' + eachep.category2 
            for (rkey, d_rid) in eachep.rid_dict.items():
                #strip the RID out.
                m = self.push_base.ridmatch_obj.match(d_rid)
                if m:
                    rid_sum += 1
                    this_rid = m.group('onerid')
                    if this_rid in self.RID_MAP:
                        if d_rid == self.RID_MAP[this_rid].fullstring:
                            rid_dup += 1
                        else:
                            rid_error += 1
                            print "RID Dup " + d_rid
                            print "        " + self.RID_MAP[this_rid].rid_base
                            one_rid = RID()
                            one_rid.reset(this_rid, "0", "", d_rid, eachep.episode_id, eachep.episode_encode, str(rkey), rid_category)
                            self.RID_MAP[this_rid] = one_rid
                            rid_added += 1 
                    else:
                        one_rid = RID()
                        one_rid.reset(this_rid, "0", "", d_rid, eachep.episode_id, eachep.episode_encode, str(rkey), rid_category)
                        self.RID_MAP[this_rid] = one_rid
                        rid_added += 1 

        print "\nSummary: %d processed\n%d rid loaded into RID_MAP\n%d dup found.\n%d ERROR FOUND" %(rid_sum, rid_added, rid_dup, rid_error) 
        print "Summary: %d episode filtered."%filter_ep_count
        return rid_added

    ##################################################
    # PUBLIC: Write the SELF.RID_MAP into XML for persistent storage.
    def WriteRIDListIntoDisk(self):
        print "----------------------------------"
        print "Start to write RID_MAP into XML..."
        rid_document = ElementTree()
        rid_el_list = Element('rid_list')
        SubElement(rid_el_list, 'time').text = self.current_maxdate 
        rid_document._setroot(rid_el_list)
        rid_se_count = 0
        for (rkey, rvalue) in self.RID_MAP.items():
            item = Element('RID')
            rid_el_list.append(item)
            SubElement(item, 'id').text = rkey
            SubElement(item, 'count').text = rvalue.rid_copies
            SubElement(item, 'time').text = rvalue.rid_time
            SubElement(item, 'fullrid').text = rvalue.fullstring
            SubElement(item, 'pri').text = rvalue.priority
            SubElement(item, 'cid').text = rvalue.content_id
            SubElement(item, 'ename').text = rvalue.encode_name
            SubElement(item, 'segno').text = rvalue.segindex
            SubElement(item, 'target').text = rvalue.push_target
            SubElement(item, 'category').text = rvalue.category
            rid_se_count += 1
            #tmparg = [rkey, rvalue.rid_copies,  rvalue.rid_time,
            #rvalue.fullstring, rvalue.priority, rvalue.content_id,
            #rvalue.encode_name, rvalue.segindex, rvalue.push_target]
            #print tmparg
            #quit()
        rid_document.write(CONFIG.VIP_DATA_RID_MAP_FILE, 'utf-8')
        print "Summary: %d rid written into %s" %(rid_se_count, CONFIG.VIP_DATA_RID_MAP_FILE) 

    ##################################################
    # PUBLIC: This is for UPDATE mode.
    # Load the RID_MAP from DISK into memory, and return the detected most
    # recent Date 
    def LoadRIDListFromDiskCache(self):
        #print the summary here.
        print "------------------------"
        print " Load RID COUNT list..."
        print "  current RID_MAP size ", len(self.RID_MAP)
        print "------------------------"
        total_rid = 0
        last_date_set = 0
        self.br_type_stat = {}
        filename = CONFIG.VIP_DATA_RID_MAP_FILE
        print "Loading " + filename
        hconfig = open(filename, 'r')
        rid_xml = ET.parse(hconfig)
        d_root = rid_xml.getroot()
        lastdate_item = d_root.find('time')
        if lastdate_item != None:
            last_date = lastdate_item.text
            last_date_set = 1
        else:
            last_date = CONFIG.VIP_DATA_DEFALT_OLD_DATE            
        print "Parsing data file shows latest data is " + last_date
        d_alli = d_root.getiterator('RID')
        filtered_rid = 0
        for rid in d_alli:
            one_rid = RID()
            one_rid.rid_base = rid.find('id').text
            one_rid.rid_copies = rid.find('count').text 
            one_rid.rid_time = rid.find('time').text
            if last_date_set == 0 and one_rid.rid_time != None:
                ctime = datetime.datetime.fromtimestamp(int(one_rid.rid_time))
                ctime_string = ctime.strftime("%Y%m%d")
                if ctime_string > last_date:
                    last_date = ctime_string
            one_rid.fullstring = rid.find('fullrid').text
            one_rid.priority = rid.find('pri').text
            one_rid.content_id = rid.find('cid').text
            one_rid.encode_name = rid.find('ename').text
            one_rid.segindex = rid.find('segno').text
            one_rid.push_target = rid.find('target').text
            one_rid.category = rid.find('category').text
            #FILTER: when loading VIPRID_store, filter out unnecessary RIDS
            if self.FilterBasedOnCategory(one_rid.category) == 1:
                filtered_rid += 1 
                continue
            if one_rid.rid_base in self.RID_MAP:
                print "Duplicate RID " + one_rid.rid_base + " Found."
            else:
                self.RID_MAP[one_rid.rid_base] = one_rid
            total_rid += 1
        print "%d RIDS loaded into RID_MAP. Lastest Data is %s" %(total_rid, last_date)
        print "%d RIDS was filtered out" %filtered_rid
        return last_date 

    ###########################################
    # PUBLIC: get the peer count number for the RID_MAP 
    def QueryRIDCount(self):
        filelist = self.push_base.DistributeRIDIntoFiles(self.RID_MAP, CONFIG.VIP_CONTROL_EXT_FILE_BASE)
        cmdlist = self.push_base.AssembleExternalCommandList(filelist)
        if len(cmdlist) <= 0:
            print "ERROR: NO external QUERY command formed, give up"
            return -1
        self.push_base.exec_commands(cmdlist)

        self.current_lookup_time = self.push_base.LoadExternalCommandResultFile(self.RID_MAP, filelist)

        if CONFIG.VIP_DATA_QUERY_DATA_BACKUP == "True":
            #we will backup the data here. 
            timenow = datetime.datetime.now()
            timestring = timenow.strftime("%Y%m%d-%H-%M")
            tarname = CONFIG.push_log_path + '/' + timestring + '.tgz'
            srcname = CONFIG.VIP_CONTROL_EXT_FILE_BASE + '*.out'

            command = "tar cfz " + tarname + " " + srcname
            print command
            os.system(command)

    ###########################################
    # PUBLIC: generate the RID task file for PUSH server.
    def GeneratePseudoVIPEpisode(self):
        #Now we have how many files. 
        print "-------------------------------"
        print "Generate PseudoVIP_Episode"
        print "-------------------------------"
        rid_document = ElementTree()
        rid_el_list = Element('rid_list')
        #SubElement(rid_el_list, 'time').text = self.current_maxdate 
        rid_document._setroot(rid_el_list)
        rid_se_count = 0
        rid_filtered_count = 0
        for (rkey, rvalue) in self.RID_MAP.items():
            #FILTER: we have several conditions to ignore the RID.
            filtered = self.FilterVIPRIDs(rvalue)
            if filtered == 1:
                rid_filtered_count += 1
                continue
            item = Element('RID')
            rid_el_list.append(item)
            SubElement(item, 'id').text = rkey
            #SubElement(item, 'count').text = rvalue.rid_copies
            #SubElement(item, 'time').text = rvalue.rid_time
            SubElement(item, 'fullrid').text = rvalue.fullstring
            SubElement(item, 'pri').text = rvalue.priority
            SubElement(item, 'cid').text = rvalue.content_id
            SubElement(item, 'ename').text = rvalue.encode_name
            SubElement(item, 'segno').text = rvalue.segindex
            SubElement(item, 'target').text = rvalue.push_target
            SubElement(item, 'category').text = rvalue.category
            rid_se_count += 1
        rid_document.write(CONFIG.VIP_CONTROL_OUTPUT_TASK_FILE, 'utf-8')
        print "Summary: %d rid written into %s" %(rid_se_count, CONFIG.VIP_CONTROL_OUTPUT_TASK_FILE) 
        print "Summary: %d rid filtered." %rid_filtered_count

    ########################################
    # We will filter out some episode to reduce work load.
    def LoadVIPFilterConfig(self):
        filename = CONFIG.VIP_CONTROL_FILTER_FILENAME
        print "---------------------------"
        print "Load VIP Filter Config file " + filename 
        print "---------------------------"
        self.vip_exclude_list = {}
        self.vip_include_list = {}
        try:
            efile = open(filename, "r")
            for line in efile.readlines():
                linename = line.rstrip(' \r\n')
                #we use UNICODE internally
                lineparts = linename.split()
                if len(lineparts) < 3:
                    continue
                if lineparts[0] != "YES" and lineparts[0] != "NO":
                    continue
                fstr = lineparts[1] + '.' + lineparts[2]
                try:
                    dfstr = fstr.decode("utf-8")
                    #dfstr = fstr
                    print "added ", dfstr
                    if lineparts[0] == "YES":
                        self.vip_include_list[dfstr] = 1
                    if lineparts[0] == "NO":
                        self.vip_exclude_list[dfstr] = 1
                except:
                    print "cannot decode string " + dfstr
            efile.close()
        except: 
            print "Failed to load VIP Filter Config file"
        print "VIP Include List:"
        print self.vip_include_list
        print "VIP Exclude List:"
        print self.vip_exclude_list

    def FilterBasedOnCategory(self, category):
        category_d = category.decode('utf-8')
        if category_d in self.vip_include_list:
            #print category, " passed."
            return 0
        if category_d in self.vip_exclude_list:
            #print category, " filtered."
            return 1
        #we split the category in half, then use ALL as the second half.
        cattwo = category_d.split('.')
        if len(cattwo) == 2:
            newcat = cattwo[0] + '.ALL'
            if newcat in self.vip_include_list:
                #print "included ", newcat, category
                return 0
            if newcat in self.vip_exclude_list:
                #print "excluded ", newcat, category 
                return 1

        if "ALL.ALL" in self.vip_exclude_list:
            #print category, " white list only, filtered."
            return 1
        return 0

    ########################################
    # We will filter out some episode to reduce work load.
    def FilterVIPEpisode(self, one_ep):
        category = one_ep.category1 + '.' + one_ep.category2
        fresult = self.FilterBasedOnCategory(category)
        if fresult == 1:
            return 1
        if len(one_ep.rid_dict) <= 2:
            return 1
        return 0 


    ########################################
    # We will filter out some RIDs to reduce work load.
    def FilterVIPRIDs(self, rvalue):
        rid_tracker_count = int(rvalue.rid_copies)
        #print rid_tracker_count,  rvalue.rid_time, self.current_lookup_time
        if rvalue.rid_time != self.current_lookup_time: 
            return 1 
        if rid_tracker_count > CONFIG.VIP_CONTROL_RID_COUNT_LOW:
            return 1 
        #We can also filter against the category here.
        if self.FilterBasedOnCategory(rvalue.category):
            return 1
        return 0 




