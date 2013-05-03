#
#

import submitter_config

from monitor_thread import monitor_thread

import os
import ConfigParser

natcheck_pid = 0

def is_already_started():
        
    try :
        if os.path.isfile( '.lock' ):
            # exist, try to del
            os.remove( '.lock' )
            
        global lock_file
        lock_file = file(".lock" , 'w+')
        return False
    except:
        return True

def start():
    if is_already_started():
        print 'instance already existing'
        return
    
    # read config
    #config = ConfigParser.ConfigParser()
    #config.read('udp_natcheck_server.config.txt')
    #port = config.get( "natcheck" , "natcheck.udp_port")
    
    # start two threads
#    s_thread = submitter_thread()
    m_thread = monitor_thread(lock_file)
    
#    s_thread.natcheck_port_ = int(port)
    
    m_thread.start()
#    s_thread.start()
    
    m_thread.join()
#    s_thread.join()

    if lock_file != None:
        lock_file.close()
        
    os.remove( '.lock' )

if __name__ == '__main__':
    start()
