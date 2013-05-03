import os
import sys
import shutil
import submitter_config

import dircache
import subprocess
import process_manager
def show_usage():
    print """
    python natcheck_control.py create_env 
    python natcheck_control.py start_all       
    python natcheck_control.py stop_all       
    python natcheck_control.py clean_all       
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
    natcheck_env_base_name = submitter_config.deploy_dir_prefix
    os.system("chmod a+x " + submitter_config.natcheck_name[3:])
    if len( sys.argv ) != 2:
        show_usage()
        return
    
    max = int(2)
	#max=2
    for i in range(max):
        env_dir = natcheck_env_base_name + str(i)
        listen_port = submitter_config.deploy_port_begin + i
        dest_config_file_name = env_dir + "/udp_natcheck_server.config.txt"
        
        dest_log_config_file_name = env_dir + "/natcheck_log.conf"
        log_file_path = submitter_config.log_path + "/" + env_dir
        print ".log path:"
        print log_file_path
        
        #os.mkdir( env_dir )
        os.system("mkdir -p " + env_dir) 
        #create log path
        os.system("mkdir -p " + log_file_path)

        # copy config file
        config_file = file("default_udp_natcheck_server.config.txt" , 'r')
        config_data = config_file.read()
        if 1 == int(i) :
            config_data = config_data.replace( "natcheck.master_ip=0.0.0.0" , "natcheck.master_ip=" + submitter_config.deploy_natcheck_ip1 )
            config_data = config_data.replace( "natcheck.diff_ip_slave_ip=0.0.0.0" , "natcheck.diff_ip_slave_ip=" + submitter_config.deploy_natcheck_ip2 )
        else:
            config_data = config_data.replace( "natcheck.master_ip=0.0.0.0" , "natcheck.master_ip=" + submitter_config.deploy_natcheck_ip2)
            config_data = config_data.replace( "natcheck.diff_ip_slave_ip=0.0.0.0" , "natcheck.diff_ip_slave_ip=" + submitter_config.deploy_natcheck_ip1)
        config_file.close()
        
        deploy_config_file = file(dest_config_file_name , 'w')
        deploy_config_file.write( config_data )
        deploy_config_file.close()
        
        # copy log file
        log_file = file("natcheck_log.conf", 'r')
        log_data = log_file.read()
        log_data = log_data.replace("log4cplus.appender.log_file.File=natcheck.log", "log4cplus.appender.log_file.File=" + log_file_path + "/natcheck.log")
        log_data = log_data.replace("log4cplus.appender.stat_log.File=natcheck_stat.log", "log4cplus.appender.stat_log.File=" + log_file_path + "/natcheck_stat.log")
        log_file.close()
        log_config_file = file(dest_log_config_file_name, 'w')
        log_config_file.write(log_data)
        log_config_file.close()

        #open(env_dir + "/qqwry.dat" , "wb").write( open("qqwry.dat" , "rb").read() )

def start_all():
    env_dirs = list_all_env_dirs()
    cwd = os.getcwd()
    
    process = []
    if process_manager.check_process() == False:
        print "natcheck service already exists !!!"
        sys.exit(0)
        
    # start process
    for env in env_dirs:
        print 'starting in ' + env + '\r\n'
        cmd_line = submitter_config.python_bin + " " + os.path.join( cwd , "natcheck_submitter.py" )
        target_cwd = os.path.join( cwd , env )
        os.chdir( target_cwd )
        new_process = subprocess.Popen( [submitter_config.python_bin , os.path.join( cwd , "natcheck_submitter.py" )] ,
 shell=False )
        process.append( new_process )
        os.chdir( cwd )
        process_manager.record_pid(str(new_process.pid))

    for p in process:
        p.wait()

def stop_all():
    process_manager.kill_all_process()
    cmd_line = submitter_config.killall_bin + " -9 natcheck"
    new_process = subprocess.Popen( cmd_line , shell=True )


def clean_all():
    cmd_line = "rm -rf natcheck_env_*"
    new_process = subprocess.Popen( cmd_line , shell=True )

def kill_all():
    stop_all()

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
    elif command == "stop_all":
        stop_all()
    elif command == "kill_all":
        kill_all()
    elif command == "clean_all":
        clean_all()
    else:
        show_usage()
        return

if __name__ == '__main__':
    parse_command()
