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
import socket

timeout = 10
socket.setdefaulttimeout(timeout)

class submitter_thread( threading.Thread ):
    def __init__(self):
        threading.Thread.__init__(self )
        self.last_submitter_time_ = None
        self.just_started_process_ = False
        
    def do_send_statics(self, host, port, post_path):
        f = file(config.post_file_name , "rb")
        data = f.read()
        f.close()
        
        headers = {
                       "Content-type": "text/xml; charset=utf-8",
                       "SOAPAction": '"http://tempuri.org/TrackerSubmitEx"',
                       "Host": host,
                       "Connection": "close",
                       "Content-Length": str(len(data)),
                       "User-Agent": "SubmitterThread(python) (compatible; MSIE 6.0; MS Web Services Client Protocol 1.1.4322.2443)"}
        conn = httplib.HTTPConnection(host , port)
        conn.request("POST", post_path, data, headers)
        response = conn.getresponse()
        cwd = os.getcwd()
        print response.status, response.reason, host, "-->" , cwd
        self.just_started_process_ = False
        conn.close()
    
    def run(self):
        self.last_submitter_time_ = time.clock() 
        while 1:
            # infinial loop
            try:
                # make xml
                self.do_send_statics(config.submit_host, config.submit_port, config.submit_post_path);
            except Exception, inst:
                print inst ,
                print 'submitter_thread: do_send_statics to server 1, exception catched'

            try:
                # make xml
                self.do_send_statics(config.submit_host2, config.submit_port2, config.submit_post_path2);
            except Exception, inst:
                print inst ,
                print 'submitter_thread: do_send_statics to server 2, exception catched'
				
            time.sleep( config.submit_interval_in_ms / 1000 )
            
if __name__ == '__main__':
    submitter = submitter_thread()
    submitter.start()
    submitter.join()
    
