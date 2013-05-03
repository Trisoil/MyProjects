import os
import sys
import shutil
import submitter_config

import dircache
import subprocess
import process_manager

def show_usage():
    print """
    python stun_control.py create_env [count]
    python stun_control.py start_all       
    python stun_control.py stop_all       
    python stun_control.py kill_all
    python stun_control.py clean_all       
    """

def list_all_env_dirs():
    list = dircache.listdir('.')
    prefix_len = len(submitter_config.deploy_dir_prefix)
    env_dirs = []
    for d in list:
        if len(d) <= prefix_len:
            continue
        
        if d[:prefix_len] == submitter_config.deploy_dir_prefix:
            env_dirs.append( d )
        
    return env_dirs

def create_env():
    # 
    tracker_env_base_name = submitter_config.deploy_dir_prefix
    if len( sys.argv ) != 3:
        show_usage()
        return
    
    max = int( sys.argv[2] )
    for i in range(max):
        env_dir = tracker_env_base_name + str(i)
        listen_port = submitter_config.deploy_port_begin + i
        dest_config_file_name = env_dir + "/udp_stun_server.config.txt"
        
        os.mkdir( env_dir )

        # copy config file
        config_file = file("default_udp_stun_server.config.txt" , 'r')
        config_data = config_file.read()
        config_data = config_data.replace( "stun.udp_port = 7000" , "stun.udp_port = " + str(listen_port) )
        config_file.close()
        
        deploy_config_file = file(dest_config_file_name , 'w')
        deploy_config_file.write( config_data )
        deploy_config_file.close()

def start_all():
    env_dirs = list_all_env_dirs()
    cwd = os.getcwd()
    processes = []
    if process_manager.check_process() == False:
        print "Stun service already exists !!!"
        sys.exit(0)
    # start process
    for env in env_dirs:
        print 'starting in ' + env + '\r\n'
        target_cwd = os.path.join( cwd , env )
        os.chdir( target_cwd )
        new_process = subprocess.Popen( [submitter_config.python_bin , os.path.join( cwd , "stun_submitter.py" )] ,
 shell=False )
        processes.append( new_process )
        os.chdir( cwd )
        process_manager.record_pid(str(new_process.pid))
    for p in processes:
      p.wait()

def kill_all():
    process_manager.kill_all_process()
    cmd_line = submitter_config.killall_bin + " -9 stunserver"
    new_process = subprocess.Popen( cmd_line , shell=True )

def clean_all():
    cmd_line = "rm -rf " + submitter_config.deploy_dir_prefix
    new_process = subprocess.Popen( cmd_line , shell=True )

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
    else:
        show_usage()
        return

if __name__ == '__main__':
    parse_command()
