#!/usr/bin/env python
# -*- coding: utf-8 -*-
#

import threading
import os
import subprocess
import time
import config

#from submitter_config import lock_file 

class monitor_thread( threading.Thread ):
    'use to monitor tracker process, if process end start it'
    
    def __init__(self ):
        threading.Thread.__init__(self )
        self.process_ = None
        
    def start_process(self):
        cwd = os.getcwd()
        exe = os.path.join(cwd , config.program_name )
        print 'starting: ' + exe
        self.process_ = subprocess.Popen( exe , shell=False , stdin = subprocess.PIPE )
        self.process_.wait()
            
        self.process_ = None
    
    def check_to_start_process(self):
        process_existing = False
        if process_existing == False:
            # start new thread
            self.start_process()
        
    def run(self): 
        while 1:
            try:
                # infinite loop
                self.check_to_start_process()
            except:
                print 'monitor_thread: exception catched'
            
            time.sleep( 1 )
