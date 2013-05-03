#coding=utf-8
import subprocess
import os
import time
import sys

import push_config

def start(num):
    while(1):
        for i in range(num):
            cmd_line = 'netstat -nultp | grep 0.0.0.0:' + str(6900 + i)
            handle = subprocess.Popen(cmd_line, shell=True, stdout=subprocess.PIPE)
            strRet = handle.communicate()[0]
            if strRet:
                continue
            else:
                cwd = os.getcwd()
                env_dir = push_config.deploy_dir_prefix + str(i)
                print "restart process " + env_dir
                target_cwd = os.path.join( cwd, env_dir )
                os.chdir(target_cwd)
                exec_cmd = os.path.join( cwd , push_config.exe_file_name )
                subprocess.Popen( exec_cmd, shell=False, stdin = subprocess.PIPE )
                os.chdir(cwd)
        cmd_line = "netstat -an | grep " + str(10000)
        handle = subprocess.Popen(cmd_line, shell=True, stdout=subprocess.PIPE)
        strRet = handle.communicate()[0]
        if not strRet:
            exec_cmd = push_config.python_bin + " hot_file_statistic_server.py 10000"
            subprocess.Popen( exec_cmd, shell=True, stdin = subprocess.PIPE) 

        time.sleep(1)

if __name__ == '__main__':
    if len(os.sys.argv) < 2:
        print "usage <python push_monitor.py [process num]>"
        sys.exit()

    process_num = int(os.sys.argv[1])
    start(process_num)

        
        

                
        
