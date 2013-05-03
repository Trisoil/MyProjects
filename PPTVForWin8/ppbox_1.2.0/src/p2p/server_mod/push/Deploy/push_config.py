#'''
#pupose: push server configuration
#author: youngkyyang
#date: 2012/02/17--2012/02/20
#'''
#coding=utf-8

#################################
PUSH_VERSION = "2.0.7"

#################################
##Deploy Related Config
deploy_port_begin = 6900
deploy_dir_prefix = "push_env_"

python_bin = "/usr/bin/python2.6"
exe_file_name = "push"

killall_bin = "/usr/bin/killall"
push_log_path = "/home/logs/push"

dir_path = "/usr/local/push"

################################
##Push Data Related Config
HOTLIST_DISK_PATH = "push_env_0"
HOTLIST_CONFIGFILE = "pushhot.clist.config"

URL_BAIDU_HOTMOVIE = "http://top.baidu.com/rss_xml.php?p=movie"
URL_DAC_TOP = 'https://data.g1d.net/dac/channel_query.jsp?s_date='
URL_DAC_TOP_PARAM = '&s_datatype=A8'
URL_DAC_INIT_LOOKBACK_RANGE = 30
URL_DRAG_SERVER = "http://drag.g1d.net/"
URL_DRAG_SERVER_PARAM = ""
URL_EPG_NEWUPLOAD = 'http://api.epg.synacast.com/xml_p2p_recent.jsp'
URL_EPG_INIIT = URL_EPG_NEWUPLOAD + '?day=5'
URL_EPG_VIP_BASE = 'http://api.epg.synacast.com/xml_p2p_vip_recent.jsp?start='

PUSHHOT_RID_EXT_FILE_BASE = "/tmp/pushhot_tracker_rid_query_set_"
PUSHHOT_RID_QUERY_DATA_BACKUP = "True"     #True to turn on data backup.
PUSHHOT_RID_QUERY_OUTPUT_FILE = "newhot_rid_makeup.xml"
PUSHHOT_RID_QUERY_TARGET = 200
PUSHHOT_RID_QUERY_PEER_LOW = 20
PUSHHOT_RID_QUERY_PEER_HIGH = 40

################################
##VIP Cold Content Push Config
VIP_CONTROL_OUTPUT_TASK_FILE = "newvip_rid_list.xml"
VIP_CONTROL_MAX_EXT_PROCESS = 10
VIP_CONTROL_MAX_RIDS_PER_FILE = 5000
VIP_CONTROL_EXT_FILE_BASE = "/tmp/push_tracker_rid_query_set_"
VIP_CONTROL_RID_TARGET = 200
VIP_CONTROL_RID_COUNT_LOW = 20
VIP_CONTROL_FILTER_FILENAME = "vip_filter.config"

VIP_DATA_DIR_NAME = "vip-data"
VIP_DATA_NAME_PREFIX = "vip_content"
VIP_DATA_RID_CACHE_NAME = "vip_rid_list"
VIP_DATA_RID_MAP_FILE = "vip_rid_status_store.xml"
VIP_DATA_DEFALT_OLD_DATE = "20090101"
VIP_DATA_DEFALT_OLD_DATE_INT = "1230768000"    #2009/1/1 in epoch format

VIP_DATA_QUERY_DATA_BACKUP = "True"     #True to turn on data backup.

##################################
##Config the external tools to query copies of RID in tracker.
VIP_EXT_BINARY = "./tracker_tool-linux-x86-gcc44-mt"
#VIP_EXT_BINARY = "ls"
VIP_EXT_ARG1_CMD = "12"
VIP_EXT_ARG3_TIMEOUT = "0.2"
VIP_EXT_ARG4_QUIT = "q"
VIP_EXT_TIMEOUT_SEC = 1800

PING_ARG1 = "-c"
PING_ARG2_NUM = "4"
PING_NUM_THREADS = 4


