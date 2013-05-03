
import os
import sys
import shutil
import dircache
import subprocess
import uuid

packet_send_bin = ".\\protocol_packets\\packet_send.exe"

def list_xml_files( dname ):
    list = dircache.listdir(dname)
    prefix_len = len(".xml")
    xml_files = []
    for d in list:
        if len(d) <= prefix_len:
            continue
        
        if d[ len(d) - prefix_len :] == ".xml":
            xml_files.append( d )
        
    
    return xml_files;

def do_make_dir( dirname ):
    try:
        os.mkdir( dirname )
    except:
        pass
    
def copy_env():
    from_env_dir = sys.argv[2]
    to_env_dir = sys.argv[3]
    copy_count = 1
    if len(sys.argv) == 5:
        copy_count = int(sys.argv[4])

    do_make_dir( to_env_dir )        
    
    to_env_prefix = to_env_dir + "/env"
    
    files = list_xml_files(from_env_dir)
    
    # initalize 
    port = 20000
    
    # 
    for i in range(copy_count):
        target_env = to_env_prefix + str(i)
        do_make_dir( target_env )
        
        for file in files:
            file_object = open( os.path.join( from_env_dir , file ) , 'r')
            try:
                xml_text = file_object.read()
                
                xml_text = xml_text.replace( "{LocalPort}" , str(port) )
                uid = str(uuid.uuid4())
                spl = uid.split("-")
                uid = spl[0] + spl[1] + spl[2] + spl[3] + spl[4]
                xml_text = xml_text.replace( "{RANDOMGUID}" , uid ) 
                
                target_file = open( os.path.join( target_env , file ) , 'w' )
                target_file.write( xml_text )
                target_file.close()
                
            finally:
                file_object.close()
                
        port += 1;

        
        # create 
        

def start_all():
    to_env_dir = sys.argv[2]
    dir_list = dircache.listdir(to_env_dir)
    for dir in dir_list:
        # 
        xml_files = list_xml_files( os.path.join( to_env_dir , dir ) )
        
        # 
        command_line = packet_send_bin
        for xml_file in xml_files:
            command_line = command_line + " " + os.path.join( to_env_dir , dir , xml_file ) 
    
        print 'starting: '
        print command_line
        subprocess.Popen( command_line , shell=True  )

def kill_all():
    print "running taskkill /f /im packet_send.exe\n"
    os.system('taskkill /f /im packet_send.exe')
    pass
    
def parse_command():
    if len(sys.argv) < 2:
        return False
    
    command = sys.argv[1]
    if command == "copy_env":
        copy_env()
    elif command == "start_all":
        start_all()
    elif command == "kill_all":
        kill_all()
    else:
        print_usage()
    
    return True

def print_usage():
    print   '''
        python packet_send_control.py copy_env from_env to_env_dir [n]
        python packet_send_control.py start_all env_dir
            '''

def run():
    if False == parse_command():
        print_usage()
        return

if __name__ == '__main__':
    run()
