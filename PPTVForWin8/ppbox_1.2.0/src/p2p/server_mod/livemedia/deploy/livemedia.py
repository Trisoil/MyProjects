import xml.dom.minidom
from time import localtime,time,strftime
import os
import sys
class StatusReader:
    def __init__(self, path):
        status_file = file(path , 'r')
        status_data = status_file.read()
        status_file.close()
        self.dom = xml.dom.minidom.parseString(status_data)
        self.status = self.dom.getElementsByTagName('live_status')[0]
    
    def get_value(self,group_name, node_name):
        target_node= self.status.getElementsByTagName(group_name)[0]
        if len(target_node.getElementsByTagName(node_name)) > 0:
            target = target_node.getElementsByTagName(node_name)[0]
            for node in target.childNodes:
                if node.nodeType == node.TEXT_NODE:
                    return node.data
        return None
    def get_attribute(self, group_name, node_name,attribute_name):
        target_node = self.status.getElementsByTagName(group_name)[0]
        return target_node.getElementsByTagName(node_name)[0].getAttribute(str(attribute_name))
    
    def get_unnormal_channel_count(self):
        channel_status = self.status.getElementsByTagName('channel_status')[0]
        channels = channel_status.getElementsByTagName('channel')
        count = 0
        for channel in channels:
            end_time = channel.getElementsByTagName('status')[0].getAttribute('end_time')
            if time() - int(end_time) > 60*10:
                count = count + 1
        return count
        
    def print_status(self,key):
        if key == 'count':
            print self.get_unnormal_channel_count()
        elif key == 'upload_speed' or key == 'user_count':
            print self.get_attribute('global_status', 'status', key)
        else:
            print self.get_value('z_packet_receive', key)
            
def parse_command():
    if len(os.sys.argv) < 2:
        return None
    # parse
    return sys.argv[1]
           
if __name__ == "__main__":
    path = 'status.xml'
    status_reader = StatusReader(path)
    if parse_command() != None:
        status_reader.print_status(parse_command())
    else:
        print 'parse command fail'