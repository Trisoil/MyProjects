#!/usr/bin/env python
# -*- coding: utf-8 -*-
#

import threading
import os
import subprocess
import time
import config
import re

#from submitter_config import lock_file 

def get_vm_size(pid):
    f = file('/proc/%s/stat' % str(pid))
    stat = f.read()
    f.close()
    stat_list = re.split(' ', stat)
    return int(stat_list[22])

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
        while self.process_.poll() is None:
            vm_size = get_vm_size(self.process_.pid)
            if vm_size > 6000000000:
                print 'vm_size=',vm_size
                print 'Killing process while its vm_size is getting too big'
                kill_process_command = 'kill -9 %(pid)s' % {'pid':self.process_.pid}
                subprocess.Popen( kill_process_command , shell=True )
                break
            time.sleep(1)
            
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
