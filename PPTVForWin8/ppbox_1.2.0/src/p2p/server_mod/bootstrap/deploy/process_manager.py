import fcntl
import subprocess
import os
def check_process():
    path = os.getcwd()
    path = path + "/pid.conf"
    print path    
    if os.path.exists(path):
        global f
        f = open( "pid.conf", "a" )
        try:
            print "try"
            fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
           #f.close()
        except IOError, e:
            print e.errno
            print "service already exists!!!"
            return False
    else:
        global f
        f = open( "pid.conf", "a" )
        fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
    return True		
    
def record_pid(pid):
    print pid
    path = os.getcwd()
    path = path + "/pid.conf"   
    f = open( "pid.conf" , "a" )
    pid_string = pid + '\n'
    f.write(pid_string)
#    fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
    f.close()
	
def kill_all_process():
    path = os.getcwd()
    path = path + "/pid.conf"
    print path    
    f = open( "pid.conf" , "r" )
    for line in f:
        cmd_line = "kill -9 " + line
        print cmd_line
        new_process = subprocess.Popen( cmd_line , shell=True )
    f.close()
    os.remove("pid.conf")

