#!/usr/bin/env python
# -*- coding: utf-8 -*-
#

import threading
import os
import time
from datetime import date
import datetime 
import submitter_config
import httplib
import string

class submitter_thread( threading.Thread ):
    def __init__(self):
        threading.Thread.__init__(self )
        self.last_submitter_time_ = None
        self.tracker_port_ = 8000
        
    def read_current_statics(self):
        statics_file = file( submitter_config.tracker_log_file_name , 'r')
        lines = statics_file.readlines()
        for line in lines:
            if len(line) < 5:
                continue
            
            r_size = len("Resource Count:")
            p_size = len("Peer Count:")
            if line[:r_size] == "Resource Count:":
                self.resource_count_ = int(line[r_size:])
            elif line[:p_size] == "Peer Count:":
                self.peer_count_ = int(line[p_size:])
                
        statics_file.close()
             
    def do_send_statics(self):
        host = submitter_config.submit_host 
        if submitter_config.submit_port != 80:
            host = host + ":" + str(submitter_config.submit_port)

        data = ""

        # 在这里判断是否重启过
        if submitter_config.just_started_process:
            data = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><soap:Body><TrackerSubmitEx new_start=\"1\" xmlns=\"http://tempuri.org/\"><peerCount>${peerCount}</peerCount><resourceCount>${resourceCount}</resourceCount><port>${port}</port><ip_tmp /><msg_tmp /></TrackerSubmitEx></soap:Body></soap:Envelope>"
        else:
            data = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><soap:Body><TrackerSubmitEx xmlns=\"http://tempuri.org/\"><peerCount>${peerCount}</peerCount><resourceCount>${resourceCount}</resourceCount><port>${port}</port><ip_tmp /><msg_tmp /></TrackerSubmitEx></soap:Body></soap:Envelope>"
            
        data = data.replace( "${resourceCount}" , str(self.resource_count_))
        data = data.replace( "${peerCount}" , str(self.peer_count_))
        data = data.replace( "${port}" , str(self.tracker_port_))
        
        headers = {
                       "Content-type": "text/xml; charset=utf-8",
                       "SOAPAction": '"http://tempuri.org/TrackerSubmitEx"',
                       "Host": host,
                       "Connection": "close",
                       "Content-Length": str(len(data)),
                       "User-Agent": "X-TrackerSubmitter (compatible; MSIE 6.0; MS Web Services Client Protocol 1.1.4322.2443)"}
        conn = httplib.HTTPConnection(host)
        conn.request("POST", submitter_config.submit_path, data, headers)
        # response = conn.getresponse()
        #print response.status, response.reason
        submitter_config.just_started_process = False
        conn.close()
    
    def run(self):
        self.last_submitter_time_ = time.clock() 
        while 1:
            # infinial loop
            try:
                self.read_current_statics()
                # make xml
                self.do_send_statics();
            except:
                print 'submitter_thread: exception catched'
            
            time.sleep( submitter_config.check_interval_in_ms / 1000 )
            
if __name__ == '__main__':
    submitter = submitter_thread()
    submitter.run()
    submitter.join()
    
