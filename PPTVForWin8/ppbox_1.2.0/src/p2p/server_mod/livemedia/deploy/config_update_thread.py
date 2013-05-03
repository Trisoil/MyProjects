#!/usr/bin/env python
# -*- coding: utf-8 -*-
#

import threading
import os
import time
from datetime import date
import datetime 
import config
import httplib
import string
import ConfigParser
import subprocess

class config_update_thread( threading.Thread ):
    def __init__(self):
        threading.Thread.__init__(self )
        self.config_updated_ = False

    def do_fetch_config(self):
        host = config.submit_host
        url_path = config.submit_config_path
        
        headers = {
                       "Host": host,
                       "Connection": "close",
                       "User-Agent": "IE (compatible; MSIE 6.0; MS Web Services Client Protocol 1.1.4322.2443)"}
        conn = httplib.HTTPConnection(host , config.submit_port)
        conn.request("GET", url_path , "", headers)
        response = conn.getresponse()
        if response.status != 200:
            print 'update failed: ' ,
            print response.status , response.reason
            return False
        
        data = response.read()

        if os.path.exists( "live_media_server.conf.update" ):
            os.remove( "live_media_server.conf.update" )

        f = file( "live_media_server.conf.update" , "wb" )
        f.write(data)
        f.close()

        port_number_changed = False
        config_old = None
        
        if os.path.exists( "live_media_server.conf" ):
            
            config_old = ConfigParser.ConfigParser()
            f = open('live_media_server.conf')
            config_old.readfp(f)
            f.close()
            os.remove( "live_media_server.conf" )
            
        os.rename( "live_media_server.conf.update" , "live_media_server.conf")
        config_new = ConfigParser.ConfigParser()
        config_new.read("live_media_server.conf")
        
        if config_old != None:
            # 原来的配置已经存在了,这里需要判断是否有端口修改,如果有,这里要应用新的配置
            if config_old.get( "p2p" , "listen_port" ) != config_new.get( "p2p" , "listen_port" ):
                # 有修改
                port_number_changed = True
        
        if port_number_changed:
            # 直接杀死程序吧
            cmd_line = config.killall_bin + " -9 " + config.program_name
            new_process = subprocess.Popen( cmd_line , shell=True )
        
        return True;
    
    def run(self):
        # 死循环去获取数据
        while True:
            try:
                if self.do_fetch_config():
                    self.config_updated_ = True
            except Exception, inst:
                print inst ,
                print 'config_update_thread: exception catched'
                
            time.sleep( config.check_config_interval_in_ms / 1000 )
