import threading
import os
import subprocess
import time

f = open("tracker_list.txt" , "r")
lines = f.readlines()

now = str(time.time())
result = open("result.txt" , 'w') 

begin_port = 18000

for l in lines:
    pos = l.find(':')
    if pos == -1:
        continue
    
    if l.find('\r') != -1:
        l = l[0:len(l) - 2]
    else:
        l = l[0:len(l) - 1]
    
    count = int(l[pos+1:])
    
    for i in range(count):
      # do test
      cwd = os.getcwd()
      exe = os.path.join(cwd , "test_tracker.exe" )
      exe = exe + " " + l[0:pos] + " " + str(begin_port + i)
      print 'starting: ' + exe
      tester_process = subprocess.Popen( exe , shell=False , stdin = subprocess.PIPE )
      # write pid to .lock
      tester_process.wait()
      result_str = l[0:pos] + ":" + str(begin_port+i) + "\t"
      
      if tester_process.returncode == 0:
          result_str = result_str + "success\n"
      else:
          result_str = result_str + "failed\n"
      
      result.write(result_str)
    
result.close()