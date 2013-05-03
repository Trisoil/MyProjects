'''
对ip出现的次数进行统计，查询出现问题的ip地址，主要用来分析日志
'''
#!/usr/bin/env python
#ecoding=utf-8
import os
import sys


class IpClassify:
    def __init__(self, file_list):
        self.file_list_ = file_list
        self.ip_times_map_ = {}

    def parse_line(self, str_line):
        start_index = str_line.find('location:')
        if (start_index == -1):
            return 
        start_index += len('location:')
        end_index = str_line.rfind('|')
        location = str_line[start_index:end_index]
        key = int(location)/65536
#        print key
        if self.ip_times_map_.has_key(key):
            self.ip_times_map_[key] += 1
        else:
            self.ip_times_map_[key] = 1

    def read_file(self, filename):
        filep = open(filename, 'r')
        for line in filep.readlines():
            self.parse_line(line)
    
    def start(self):
        for filename in self.file_list_:
            self.read_file(filename)

    def output(self):
        for item in self.ip_times_map_:
            print str(item) + ":" + str(self.ip_times_map_[item])

if __name__ == '__main__':
    if len(os.sys.argv) < 2:
        print "please input classify files"
    else:
        files = sys.argv[1:]
        ip_classify = IpClassify(files)
        ip_classify.start()
        ip_classify.output()
