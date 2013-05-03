
import threading
import submitter_config
import os
import subprocess
import time

#from submitter_config import lock_file 

class monitor_thread( threading.Thread ):
    'use to monitor tracker process, if process end start it'
    
    def __init__(self , lock_file):
        threading.Thread.__init__(self )
        self.tracker_process_ = None
        
    def start_tracker_process(self):
        cwd = os.getcwd()
        exe = os.path.join(cwd , submitter_config.tracker_name )
        print 'starting: ' + exe
        self.tracker_process_ = subprocess.Popen( exe , shell=False , stdin = subprocess.PIPE )
        # write pid to .lock
        submitter_config.just_started_process = True
        self.tracker_process_.wait()
        self.tracker_process_ = None
    
    def check_to_start_tracker(self):
        process_existing = False
        if process_existing == False:
            # start new thread
            self.start_tracker_process()
        
    def run(self): 
        while 1:
            try:
                # infinial loop
                self.check_to_start_tracker()
                time.sleep( 1 )
            except:
                print 'monitor_thread: exception catched'
            
            time.sleep( 1 )
