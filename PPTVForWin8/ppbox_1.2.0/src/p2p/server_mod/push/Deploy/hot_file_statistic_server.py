#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import sys
import datetime
import re

def get_current_day():
    now = datetime.datetime.now()
    return now.strftime("%Y-%m-%d")

TOP_HOT_VIDEO_NUM = 400

file_name_to_play_count_dic = {}

current_day = get_current_day()
recent_play_history_num = 3
write_file_threshold_value = 10000

def start_udp_server(port):
    global write_file_threshold_value
    global file_name_to_play_count_dic
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', port))

    total = 0
    i = 0
    while True:
        try:
            hot_file_lines, (addr, p) = s.recvfrom(1500)
            handle_received_lines(hot_file_lines)
            i += 1
            total += 1
            if i >= write_file_threshold_value:
                write_sorted_hot_video(get_current_day(), file_name_to_play_count_dic)
                i = 0
        except:
            continue
        
def handle_received_lines(lines):
    global file_name_to_play_count_dic
    global current_day
    global recent_play_history_num
    try:
        hot_file_list = lines.split()
        i = 0
        for line in hot_file_list:
            i += 1
            if i == recent_play_history_num:
                break
            if line.find("mp4.ft") != -1:
                #this is new version, it is in UTF-8 codec already.
                origin_name = line.strip()
            else:
                #this is old version, conversion required.
                origin_name = line.strip().decode("GBK").encode("UTF-8")
            name_ok, name = get_video_name(origin_name)
            if not name_ok:
                continue
            if get_current_day() == current_day:
                add_video_play_times(name)
            else:
                write_sorted_hot_video(current_day, file_name_to_play_count_dic)
                current_day = get_current_day()
                file_name_to_play_count_dic.clear()
                add_video_play_times(name)
    except:
        return

def add_video_play_times(name):
    global file_name_to_play_count_dic
    if file_name_to_play_count_dic.has_key(name):
        file_name_to_play_count_dic[name] = file_name_to_play_count_dic[name] + 1
    else:
        file_name_to_play_count_dic[name] = 1

def get_video_name(name_with_segment):
    try:
        pattern = "(.+)\[\d+\]\.mp4(.*)"
        m = re.match(pattern, name_with_segment)
        if m:
            return (True, m.group(1) + ".mp4")
        return (False, "")
    except:
        return (False, "")

def write_sorted_hot_video(current_day, play_count_dic):
    global TOP_HOT_VIDEO_NUM
    sorted_list = sorted(play_count_dic.iteritems(), key=lambda d:d[1], reverse=True)
    head_str = "类型\tGUID/VideoID\t频道名\tVV\tUV\tWT\tWT-AVG\t新版本VV\t缓冲次数总数（次）\t平均缓冲次数（次）"
    fo = open("hot-" + current_day + ".txt", "w")
    fo.write(head_str+"\n")
    for i in range(len(sorted_list)):
        if i < TOP_HOT_VIDEO_NUM:
            cur_line = "开放高清\t" + sorted_list[i][0] + "\t" + sorted_list[i][0].strip(".mp4") + "\t"\
                + str(sorted_list[i][1]) + "\t" + str(sorted_list[i][1]) + "\t" + "30\t30\t" + str(sorted_list[i][1]) + "\t"\
                + "25000\t2.5\t"
            fo.write(cur_line+"\n")
        else:
            break
    fo.close()
    
if __name__ == '__main__':
    if len(sys.argv) == 2:
        start_udp_server(int(sys.argv[1]))
    else:
        print "usage: hot_file_statistic_server <port>"
    
