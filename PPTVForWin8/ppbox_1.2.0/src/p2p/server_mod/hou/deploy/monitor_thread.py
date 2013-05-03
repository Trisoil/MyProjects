
import threading
import submitter_config
import os
import subprocess
import time
import submitter_thread

class monitor_thread( threading.Thread ):
    'use to monitor process, if process end start it'
    
    def __init__(self):
        threading.Thread.__init__(self )
        self.tracker_process_ = None
        
    def start_process(self):
        cwd = os.getcwd()
        exe = os.path.join(cwd , submitter_config.hou_name )
        print 'starting: ' + exe
        self.tracker_process_ = subprocess.Popen( exe , shell=False , stdin = subprocess.PIPE )
        # write pid to .lock
        self.tracker_process_.wait()
        self.tracker_process_ = None
    
    def check_to_start_process(self):
        self.start_process()
        
    def run(self): 
        while 1:
            try:
                # infinial loop
                self.check_to_start_process()
                time.sleep( 1 )
            except:
                print 'monitor_thread: exception catched'
            
            time.sleep( 1 )
