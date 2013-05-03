import xml.dom.minidom
import os
import sys
class StatusReader:
    def __init__(self, path):
        status_file = file(path , 'r')
        status_data = status_file.read()
        status_file.close()
        self.dom = xml.dom.minidom.parseString(status_data)
        self.status = self.dom.getElementsByTagName('hou_status')[0]
    
    def get_status(self,group_name, node_name):
        target_node= self.status.getElementsByTagName(group_name)[0]
        if len(target_node.getElementsByTagName(node_name)) > 0:
            target = target_node.getElementsByTagName(node_name)[0]
            for node in target.childNodes:
                if node.nodeType == node.TEXT_NODE:
                    return node.data
        return None
        
    def print_status(self,key):
        if key == 'drop_rate':
            packet_received = self.get_status('period_num', 'packet_received')
            packet_dropped = self.get_status('period_num', 'packet_dropped')
            if packet_received != None and packet_dropped != None:
                print int(packet_dropped) * 100 / int(packet_received)
            return 
        elif self.get_status('period_num', key) != None:
            print self.get_status('period_num', key)
        elif self.get_status('packet_num', key) != None:
            print self.get_status('packet_num', key)
        else:
            print 'invalid key'
            
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
            