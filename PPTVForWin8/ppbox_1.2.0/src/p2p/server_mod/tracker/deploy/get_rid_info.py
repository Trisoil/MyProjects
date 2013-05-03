'''
根据RID查找对应的广告名称，搜索get_episode_info.py获取的RID信息，实现通过RID反查广告名称等信息
'''
#!/usr/bin/env python
#ecoding=utf-8
import os
import sys
import subprocess

class RidInfo:
    def __init__(self, rid, from_time, to_time):
        self.rid_ = rid
        self.from_time_ = from_time
        self.to_time_ = to_time
        self.area_ = []

    

def start(str_rid):
    cmd_line = "grep " + str_rid + " *"
    handle = subprocess.Popen(cmd_line, shell=True, stdout=subprocess.PIPE)
    strRet = handle.communicate()[0]
    if strRet:
        print strRet
    else:
        return

if __name__ == '__main__':
    if len(os.sys.argv) < 2:
        print 'usage <python get_rid_info.py RID>'
        sys.exit()

    start(sys.argv[1])
