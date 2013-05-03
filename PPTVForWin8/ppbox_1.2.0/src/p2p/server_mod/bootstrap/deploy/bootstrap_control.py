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
import process_manager

def print_help():
    print '''
usage:
        python bootstrap_control.py start
        python bootstrap_control.py stop
    '''
def start():
    if process_manager.check_process() == False:
         sys.exit(0)
    process_manager.record_pid(str(os.getpid()))

    m_thread = monitor_thread.monitor_thread()

    m_thread.start()

    m_thread.join()

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
        
    
    
