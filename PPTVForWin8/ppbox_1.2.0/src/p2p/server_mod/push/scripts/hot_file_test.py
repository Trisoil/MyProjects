from socket import *
import string
import time
import sys

host = "127.0.0.1"
port = 10000 
data_file = "hot_list.txt"
numtimes = 5500

if len(sys.argv) >= 4 :
    host = sys.argv[1] 
    port = int(sys.argv[2])
    data_file = sys.argv[3]
    numtimes = int(sys.argv[4])
else:
    print "\n"
    print "Usage: hot_file_test.py <host-ip> <port> <data_file_name> <copies>"
    print " For example:"
    print "      hot_file_test.py 127.0.0.1 10000 hot_list.txt 6000"
    print "\n"
    quit()

UDPSock = socket(AF_INET,SOCK_DGRAM)


print "\nOur target:"
print "server running on %s port %s" % (host, port)

string_vector = []
ff = open(data_file, 'r')
for line in ff.readlines():
    linename = line.rstrip(' \r\n')
    string_vector.append(linename)
ff.close()

test_data = ""
for eachline in string_vector:
    test_data = test_data + "\n" + eachline

print "Test data is "
print test_data

try:
  #the resetter...
  #UDPSock.sendto(test_data, (host,port))
  
  for x in range(numtimes):
      if(UDPSock.sendto(test_data,(host,port))):
          print "*",
      else:
          print "."
              
      time.sleep(0.0001)
  print "Done."

except:
  print "Send failed"


UDPSock.close()
