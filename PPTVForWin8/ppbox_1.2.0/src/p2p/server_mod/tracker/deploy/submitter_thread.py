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
import socket

def generate_http_request(host, path, headers, data = ""):

    s = ""
    if len(data) > 0:
        s = "POST "
        if not headers.has_key("Content-Length"):
            headers["Content-Length"] = str(len(data))
            
    else:
        s = "GET "
    s += path
    s += " HTTP/1.1\r\n"
    for k,j in headers.items():
        s += k + ": " + j + "\r\n"
    s += "\r\n"
    s += data

    return s

def http_request(host, port, path, headers, data, timeout=10):

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(timeout)
    sock.connect((host, port))
    sock.send(generate_http_request(host, path, headers, data))
    response = ""
    
    while True:
        s = sock.recv(1024)
        if len(s) > 0:
            response += s
        else:
            break
        
    sock.close()
    
    return response

class submitter_thread( threading.Thread ):
    def __init__(self):
        threading.Thread.__init__(self )
        self.last_submitter_time_ = None
        self.tracker_port_ = 8000
        self.resource_count_ = 0
        self.peer_count_ = 0
        self.just_started_process_ = False
        
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

        data = ""

        # 在这里判断是否重启过
        if self.just_started_process_ == True:
            data = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><soap:Body><TrackerSubmitEx new_start=\"1\" xmlns=\"http://tempuri.org/\"><peerCount>${peerCount}</peerCount><resourceCount>${resourceCount}</resourceCount><port>${port}</port><ip_tmp /><msg_tmp /></TrackerSubmitEx></soap:Body></soap:Envelope>"
        else:
            data = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><soap:Body><TrackerSubmitEx xmlns=\"http://tempuri.org/\"><peerCount>${peerCount}</peerCount><resourceCount>${resourceCount}</resourceCount><port>${port}</port><ip_tmp /><msg_tmp /></TrackerSubmitEx></soap:Body></soap:Envelope>"
            
        data = data.replace( "${resourceCount}" , str(self.resource_count_))
        data = data.replace( "${peerCount}" , str(self.peer_count_))
        data = data.replace( "${port}" , str(self.tracker_port_))
        
        headers = {
                       "Content-type": "text/xml; charset=utf-8",
                       "SOAPAction": '"http://tempuri.org/TrackerSubmitEx"',
                       "Host": submitter_config.submit_host,
                       "Connection": "close",
                       "Content-Length": str(len(data)),
                       "User-Agent": "X-TrackerSubmitter (compatible; MSIE 6.0; MS Web Services Client Protocol 1.1.4322.2443)"}
        resp = http_request(submitter_config.submit_host, submitter_config.submit_port, submitter_config.submit_path, headers, data, 5)
        cwd = os.getcwd()
        print resp, "-->" , cwd
        self.just_started_process_ = False
    
    def run(self):
        self.last_submitter_time_ = time.clock() 
        while 1:
            # infinial loop
            try:
                self.read_current_statics()
                # make xml
                self.do_send_statics()
            except:
                print 'submitter_thread: exception catched'
            
            time.sleep( submitter_config.check_interval_in_ms / 1000 )
            
if __name__ == '__main__':
    submitter = submitter_thread()
    submitter.run()
    submitter.join()
    
