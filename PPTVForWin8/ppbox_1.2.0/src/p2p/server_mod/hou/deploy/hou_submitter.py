import submitter_config

from monitor_thread import monitor_thread
from submitter_thread import submitter_thread

import os
import ConfigParser

#tracker_pid = 0


def start():

    # read config
    #config = ConfigParser.ConfigParser()
    #config.read('hou.conf')
    #port = config.get( "HouServer" , "port")
    
    # start two threads
    #s_thread = submitter_thread()
    m_thread = monitor_thread()
    
    #s_thread.tracker_port_ = int(port)
    
    m_thread.start()
    #s_thread.start()
    
    m_thread.join()
    #s_thread.join()

if __name__ == '__main__':
    start()
