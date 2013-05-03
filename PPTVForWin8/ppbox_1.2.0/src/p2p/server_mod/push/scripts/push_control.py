#'''
#pupose: control push server running, create dirs, start push process, kill process, clean files
#author: youngkyyang
#date: 2012/02/17--2012/02/20
#'''
#coding=utf-8
import os
import shutil
import dircache
import sys
import subprocess

import push_config

def show_usage():
    print """
    python push_control.py create_env [count]
    python push_control.py start_all             
    python push_control.py kill_all
    python push_control.py clean_all
    python push_control.py re_data
    python push_control.py update_config [count]
                #update push_server.conf and push_logging.conf to push_env_*
    python push_control.py update_data [count]
                #update pushhot.clist.config to push_env_*
    python push_control.py update_inputdata [count] [filename]
                #<filename> is default to ALL, you can also specify the filename
                #update newupload_content_list.xml, newhot._content_list.xml and/or newvip_rid_list.xml to push_env_*
    """

def list_all_env_dirs():
    list = dircache.listdir('.')
    prefix_len = len(push_config.deploy_dir_prefix)
    env_dirs = []
    for d in list:
        if len(d) <= prefix_len:
            continue
        
        if d[:prefix_len] == push_config.deploy_dir_prefix:
            env_dirs.append( d )
        
    return env_dirs

def create_env():
    push_env_base_name = push_config.deploy_dir_prefix
    if len( sys.argv ) != 3:
        show_usage()
        return

    cmd_line = "mkdir -p " + push_config.push_log_path
    print cmd_line
    os.system(cmd_line)
    
    max = int(sys.argv[2])
    for i in range(max):
        env_dir = push_env_base_name + str(i)
        listen_port = push_config.deploy_port_begin + i;
        dest_config_file_name = env_dir + "/push_server.conf"
        dest_log_config_file_name = env_dir + "/push_logging.conf"
        log_file_path = push_config.push_log_path + "/" + push_config.deploy_dir_prefix + str(i)
        print log_file_path

        os.mkdir(env_dir)
        
        #create log path
        os.system("mkdir -p " + log_file_path)

        # copy config file
        config_file = file("push_server.conf", 'r')
        config_data = config_file.read()
        config_data = config_data.replace( "UDP_PORT=6900", "UDP_PORT=" + str(listen_port))
        config_file.close()

        deploy_config_file = file(dest_config_file_name, 'w')
        deploy_config_file.write( config_data )
        deploy_config_file.close()

        # copy log file
        log_file = file("push_logging.conf", 'r')
        log_data = log_file.read()
        log_data = log_data.replace("log4cplus.appender.log_file.File=pushserver.hot.log", "log4cplus.appender.log_file.File=" + log_file_path + "/pushserver.hot.log")
        log_data = log_data.replace("log4cplus.appender.plog.File=statspush.log", "log4cplus.appender.plog.File=" + log_file_path + "/statspush.log")
        log_data = log_data.replace("log4cplus.appender.stat_log.File=pushstatistics.log", "log4cplus.appender.stat_log.File=" + log_file_path + "/pushstatistics.log")
        log_data = log_data.replace("log4cplus.appender.vip_log.File=viphistory.log", "log4cplus.appender.vip_log.File=" + log_file_path + "/viphistory.log")
        log_file.close()
        log_config_file = file(dest_log_config_file_name, 'w')
        log_config_file.write(log_data)
        log_config_file.close()

        #copy other file
        try:
            shutil.copy('newvip_rid_list.xml', env_dir + '/newvip_rid_list.xml')
        except:
            print "cannot copy newvip_rid_list.xml to ", env_dir
        try:
            shutil.copy('newhot_content_list.xml', env_dir + '/newhot_content_list.xml')
        except:
            print "cannot copy newhot_content_list.xml to ", env_dir

        try:
            shutil.copy('newupload_content_list.xml', env_dir + '/newupload_content_list.xml')
        except:
            print "cannot copy newupload_content_list.xml to ", env_dir
        try:
            shutil.copy('newhot_rid_makeup.xml', env_dir + '/newhot_rid_makeup.xml')
        except:
            print "cannot copy newhot_rid_makeup.xml to ", env_dir
            
        
def update_config():
    push_env_base_name = push_config.deploy_dir_prefix
    if len( sys.argv ) != 3:
        show_usage()
        return

    max = int(sys.argv[2])
    for i in range(max):
        env_dir = push_env_base_name + str(i)
        listen_port = push_config.deploy_port_begin + i;
        dest_config_file_name = env_dir + "/push_server.conf"
        dest_log_config_file_name = env_dir + "/push_logging.conf"
        log_file_path = push_config.push_log_path + "/" + push_config.deploy_dir_prefix + str(i)
        print log_file_path

        # copy config file
        config_file = file("push_server.conf", 'r')
        config_data = config_file.read()
        config_data = config_data.replace( "UDP_PORT=6900", "UDP_PORT=" + str(listen_port))
        config_file.close()

        deploy_config_file = file(dest_config_file_name, 'w')
        deploy_config_file.write( config_data )
        deploy_config_file.close()

        # copy log file
        log_file = file("push_logging.conf", 'r')
        log_data = log_file.read()
        log_data = log_data.replace("log4cplus.appender.log_file.File=pushserver.hot.log", "log4cplus.appender.log_file.File=" + log_file_path + "/pushserver.hot.log")
        log_data = log_data.replace("log4cplus.appender.plog.File=statspush.log", "log4cplus.appender.plog.File=" + log_file_path + "/statspush.log")
        log_data = log_data.replace("log4cplus.appender.stat_log.File=pushstatistics.log", "log4cplus.appender.stat_log.File=" + log_file_path + "/pushstatistics.log")
        log_data = log_data.replace("log4cplus.appender.vip_log.File=viphistory.log", "log4cplus.appender.vip_log.File=" + log_file_path + "/viphistory.log")
        log_file.close()
        log_config_file = file(dest_log_config_file_name, 'w')
        log_config_file.write(log_data)
        log_config_file.close()


def update_data():
    push_env_base_name = push_config.deploy_dir_prefix
    if len( sys.argv ) != 3:
        show_usage()
        return

    max = int(sys.argv[2])
    for i in range(max):
        env_dir = push_env_base_name + str(i)
        dest_config_file_name = env_dir + "/pushhot.clist.config"
        print "Copy pushhot.clist.config to" + dest_config_file_name 
        try:
            shutil.copy('pushhot.clist.config', dest_config_file_name)
        except:
            print "copy failed ", dest_config_file_name
        dest_config_file_name = env_dir + "/pushhot.viprid.config"
        print "Copy pushhot.viprid.config to" + dest_config_file_name
        try:
            shutil.copy('pushhot.viprid.config', dest_config_file_name)
        except:
            print "copy failed ", dest_config_file_name
            

def update_inputdata():
    push_env_base_name = push_config.deploy_dir_prefix
    if len( sys.argv ) < 3:
        show_usage()
        return

    max = int(sys.argv[2])
    if len(sys.argv) >= 4:
        filelist = sys.argv[3]
    else:
        filelist = 'ALL'
    for i in range(max):
        env_dir = push_env_base_name + str(i)
        if filelist == 'ALL':
            try:
                print "Copy newip_rid_list.xml to " + env_dir
                shutil.copy('newvip_rid_list.xml', env_dir + '/newvip_rid_list.xml')
            except:
                print "failed newip_rid_list.xml" 
            try:
                print "Copy newhot_content_list.xml to " + env_dir
                shutil.copy('newhot_content_list.xml', env_dir + '/newhot_content_list.xml')
            except:
                print "failed newhot_content_list.xml" 
                
            try:
                print "Copy newupload_content_list.xml to " + env_dir
                shutil.copy('newupload_content_list.xml', env_dir + '/newupload_content_list.xml')
            except:
                print "failed newupload_content_list.xml" 
                
            try:
                print "Copy newhot_rid_makeup.xml to " + env_dir
                shutil.copy('newhot_rid_makeup.xml', env_dir + '/newhot_rid_makeup.xml')
            except:
                print "failed newhot_rid_makeup.xml" 
                
        else:
            print "Copy " + filelist + " to " + env_dir
            shutil.copy(filelist, env_dir + '/' + filelist)
            
 
def start_all():
    env_dirs = list_all_env_dirs()

    process_num = len(env_dirs)
    os.system( push_config.python_bin + " push_monitor.py " + str(process_num)+" &")


def kill_all():
    print "kill monitor process"
    shell_cmd = "ps -ef | grep 'push_monitor.py'"
    handle = subprocess.Popen(shell_cmd, shell=True, stdout=subprocess.PIPE)
    strRet = handle.communicate()[0]

    lines = strRet.splitlines()
    for line in lines:
        ret = line.find("python")
        if ret == -1:
            continue
        else:
            words = line.split()
            os.system("kill -9 " + words[1])
            
    shell_cmd = "ps -ef | grep 'hot_file_statistic_server.py'"
    handle = subprocess.Popen(shell_cmd, shell=True, stdout=subprocess.PIPE)
    strRet = handle.communicate()[0]
    lines = strRet.splitlines()
    for line in lines:
        ret = line.find("python")
        if ret == -1:
            continue
        else:
            words = line.split()
            os.system("kill -9 " + words[1])
            
    print "kill all process"
    cmd_line = "killall -9 " + push_config.exe_file_name
    os.system(cmd_line)

def clean_all():
    cmd = "rm -rf " + push_config.deploy_dir_prefix + "*"
    os.system(cmd)

def retrieve_data():
    os.chdir(push_config.dir_path)
#    os.system("python PushRetrieveData.py 0")
    os.system(push_config.python_bin + " PushRetrieveData.py 0")
    env_dirs = list_all_env_dirs()
    cwd = os.getcwd()

    for env in env_dirs:
        target_cwd = os.path.join( cwd, env)
        os.system("cp newhot_content_list.xml " + target_cwd + "/")
        os.system("cp newupload_content_list.xml " + target_cwd + "/")
        os.system("cp newvip_rid_list.xml " + target_cwd + "/")
        os.system("cp newhot_rid_makeup.xml " + target_cwd + "/")
    
def parse_command():
    if len(os.sys.argv) < 2:
        show_usage()
        return

    # parse
    command = sys.argv[1]
    if command == "create_env":
        create_env()
    elif command == "start_all":
        start_all()
    elif command == "kill_all":
        kill_all()
    elif command == "clean_all":
        clean_all()
    elif command == "re_data":
        retrieve_data()
    elif command == "update_config":
        update_config()
    elif command == "update_data":
        update_data()
    elif command == "update_inputdata":
        update_inputdata()
    else:
        show_usage()
        return

if __name__ == '__main__':
    parse_command()
