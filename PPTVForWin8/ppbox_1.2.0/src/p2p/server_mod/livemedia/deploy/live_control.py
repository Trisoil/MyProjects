#!/usr/bin/env python
# -*- coding: utf-8 -*-
#

import os
import sys
import httplib
import subprocess
import time
import fcntl
import config
import monitor_thread
import submitter_thread
import process_manager

from config_update_thread import config_update_thread

def print_help():
    print '''
usage:
        python live_control.py start
        python live_control.py stop
    '''
def start():
    if process_manager.check_process() == False:
         sys.exit(0)
    process_manager.record_pid(str(os.getpid()))
    u_thread = config_update_thread()
    u_thread.start()
    while u_thread.config_updated_ == False:
        if not u_thread.isAlive():
            print 'update config thread is dead!!!'
            sys.exit(-1)
        else:
            time.sleep(1)

    m_thread = monitor_thread.monitor_thread()
    s_thread = submitter_thread.submitter_thread()

    m_thread.start()
    s_thread.start()

    m_thread.join()
    s_thread.join()
    u_thread.join()

def stop():
    process_manager.kill_all_process()
    cmd_line = config.killall_bin + " -9 " + config.program_name
    new_process = subprocess.Popen( cmd_line , shell=True )
    
   
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_help()
        sys.exit(-1)
    
    command = sys.argv[1]
    if command == "start":
        start()
    elif command == "stop":
        stop()
    else:
        print_help()
        sys.exit(-1)
        
    
    
