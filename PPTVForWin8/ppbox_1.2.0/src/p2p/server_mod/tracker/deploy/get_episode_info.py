'''
通过url拉取对应的广告的信息，然后解析拉取的信息，获取广告的资源rid，广告名称name，
播放时间playtime，文件长度filelength，到期时间expire_date，长度length。将解析后的
信息保存在map中，map的key是rid，value是RidInfo，解析完成之后按照ip进行分区保存在
文件中，文件名的前缀是rid_info
'''
#!/usr/bin/env python
#ecoding=utf-8
import urllib2
import os
import sys
import re
import time

class RidInfo:
    def __init__(self, rid, url, playtime, filelength, name, expire_date, length):
        self.rid_ = rid
        self.url_ = url
        self.playtime_ = playtime
        self.filelength_ = filelength
        self.name_ = name
        self.expire_date_ = expire_date
        self.length_ = length
    
    def display(self):
        print "rid:" + self.rid_ 
        print "name:" + self.name_ 
        print "playtime:" + self.playtime_ 
        print "filelength:" + self.filelength_
        print "expire_date:" + self.expire_date_ 
        print "length:" + self.length_ 
        print "url:" + self.url_
        
    def save_to_file(self, filep):
        filep.write(self.rid_ + "|" + self.name_ + "|" + self.playtime_ + "|" + self.filelength_ + "|" + self.expire_date_ + "|" + self.length_ + "|" + self.url_)

class RidInfoManager:
    def __init__(self, str_ip = "", str_area_info = ""):
        self.rid_info_map_ = {}
        self.ip_ = str_ip
        self.area_info_ = str_area_info

    def retrieve_url(self, url, retries, ssec = 1):
        count = retries
        while count > 0 :
            ecode = 0
            try:
                response = urllib2.urlopen(url)
                return response
            except urllib2.HTTPError, e:
                ecode = e.code

            count -= 1

            if ecode != 404:
                date.sleep(ssec)
            else:
                break

        print "cannot retrive retrieve ", url
        return None

    def parse_rid(self, str_rid_data):
        start_index = str_rid_data.find("<FileInfo>")
        if (start_index == -1):
            return None

        start_index += len("<FileInfo>")
        end_index = str_rid_data.find("</FileInfo>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        
        start_index = data.find("rid=")
        if (start_index == -1):
            return None

        start_index += len("rid=")
        end_index = data.find("&amp")
        if (end_index == -1):
            return None

        rid = data[start_index:end_index]

        return rid
    
    def parse_url(self, str_rid_data):
        start_index = str_rid_data.find("<URL>")
        if (start_index == -1):
            return None

        start_index += len("<URL>")
        end_index = str_rid_data.find("</URL>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        url = data 
        return url

    def parse_playtime(self, str_rid_data):
        start_index = str_rid_data.find("<PlayTime>")
        if (start_index == -1):
            return None

        start_index += len("<PlayTime>")
        end_index = str_rid_data.find("</PlayTime>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        playtime = data 
        return playtime

    def parse_filelength(self, str_rid_data):
        start_index = str_rid_data.find("<FileInfo>")
        if (start_index == -1):
            return None

        start_index += len("<FileInfo>")
        end_index = str_rid_data.find("</FileInfo>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        
        start_index = data.find("filelength=")
        if (start_index == -1):
            return None

        start_index += len("filelength=")
        end_index = data.find("&amp", start_index)
        if (end_index == -1):
            return None

        filelength = data[start_index:end_index]
        return filelength

    def parse_name(self, str_rid_data):
        start_index = str_rid_data.find("<Name>")
        if (start_index == -1):
            return None

        start_index += len("<Name>")
        end_index = str_rid_data.find("</Name>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        name = data
        return name

    def parse_expire_date(self, str_rid_data):
        start_index = str_rid_data.find("<ExpireDate>")
        if (start_index == -1):
            return None

        start_index += len("<ExpireDate>")
        end_index = str_rid_data.find("</ExpireDate>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        expire_date = data 
        return expire_date

    def parse_length(self, str_rid_data):
        start_index = str_rid_data.find("<Length>")
        if (start_index == -1):
            return None

        start_index += len("<Length>")
        end_index = str_rid_data.find("</Length>")
        if (end_index == -1):
            return None

        data = str_rid_data[start_index:end_index]
        length = data 
        return length

    def parse_rid_info(self, str_rid_data):
        rid = self.parse_rid(str_rid_data)
        if not rid:
            print "cannot parse rid"
#            print str_rid_data
            return
        
        url = self.parse_url(str_rid_data)
        if not url:
            print "cannot parse url"
            print str_rid_data
            return

        playtime = self.parse_playtime(str_rid_data)
        if not playtime:
            print "cannot parse playtime"
            print str_rid_data
            return

        filelength = self.parse_filelength(str_rid_data)
        if not filelength:
            print "cannot parse filelength"
            print str_rid_data
            return

        name = self.parse_name(str_rid_data)
        if not name:
            print "cannot parse name"
            print str_rid_data
            return

        expire_date = self.parse_expire_date(str_rid_data)
        if not expire_date:
            print "cannot parse expire_date"
            print str_rid_data
            return

        length = self.parse_length(str_rid_data)
        if not length:
            print "cannot parse length"
            print str_rid_data
            return
            
        rid_info = RidInfo(rid, url, playtime, filelength, name, expire_date, length)
#        rid_info.display()
        self.rid_info_map_[rid_info.rid_] = rid_info

    def get_content(self, url):
        response = self.retrieve_url(url, 3)
        if (response is None):
            print "failed get data, give up"
            return

        data = response.read()
        response_data_list = data.split('<VideoAD>')
        response_data_list = response_data_list[1:]
#        self.parse_rid_info(response_data_list[0])
        for edata in response_data_list:
            self.parse_rid_info(edata)
    
    def display(self):
        for index in self.rid_info_map_:
            print "\n"
            self.rid_info_map_[index].display()

    def save_file(self, filep):
        for index in self.rid_info_map_:
            self.rid_info_map_[index].save_to_file(filep)
            filep.write("|")
            filep.write(self.ip_ + "|" + self.area_info_)

    def start(self):
        url = "http://de.as.pplive.cn/ppadcontrolclient/mlist" + "?ip=" + self.ip_
        print url
        self.get_content(url)

class AreaRidManager:
    def __init__(self):
        self.area_map_ = {}

    def load_file(self, filename):
        ip_file = open(filename, 'r');
        lines = ip_file.readlines()
        return lines

    def get_rid_info_by_ip(self, str_ip, str_area_info):
        rid_info_manager = RidInfoManager(str_ip, str_area_info)    
        rid_info_manager.start()
        self.area_map_[str_ip] = rid_info_manager

    def start(self, filename):
        lines = self.load_file(filename)
        pattern = re.compile('((?:(?:25[0-5]|2[0-4]\d|[01]?\d?\d)\.){3}(?:25[0-5]|2[0-4]\d|[01]?\d?\d))')
        for line in lines:
#            match = pattern.match(line)
            match = pattern.search(line)
            if match:
                ip = match.group()
                print ip
                self.get_rid_info_by_ip(ip, line)
            else:
                print "match ip error!"
                print line
                continue

    def save_file(self, filename = "/home/pplive/youngky/work/rid_info"):
        str_time = time.strftime('%Y_%m_%d_%H', time.localtime())
        tmp_file_name = filename + str_time +".txt"
        filep = open(tmp_file_name, 'w')
        for item in self.area_map_:
            self.area_map_[item].save_file(filep)

#输入参数是ip地址列表ip.txt，如果没有输入参数，则采用默认值，否则按照参数进行输入
if __name__ == '__main__':
    ip_file = "/home/pplive/youngky/work/ip.txt"
    if len(os.sys.argv) > 1:
        ip_file = sys.argv[1]

    area_rid_manager = AreaRidManager()
    area_rid_manager.start(ip_file)
    area_rid_manager.save_file()
