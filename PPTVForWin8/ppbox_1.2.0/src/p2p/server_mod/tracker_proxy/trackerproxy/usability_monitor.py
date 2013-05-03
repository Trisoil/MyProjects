#!/usr/bin/env python
import urllib2
import thread
import threading
import array
import time
import sys
import socket
socket.setdefaulttimeout(1)
rid = ['90E3352DF6AFF8E7A8985162B834E525','60407BB086E993994CFB4FF7B8C1F260','19CE63CF20FC808483FAA16C4E370A35','BFF95BD4FDFB38FD95FB92C3FCCC2B87']
success_times = [0 for i in range(0,100)]
#total_success_times = 0
def access_cgi(success_times,i):
     try:
        url = 'http://127.0.0.1/trackercgi?rid='+rid[i%4]+'&pid=A5415160B93147369F345713BE501A5C&count=200'
        total_times = 100		
	if 1:     
                  request = urllib2.Request(url)
		  a = urllib2.urlopen(request) 
     except urllib2.HTTPError,e:
                  #print e.code
                  return
     except urllib2.URLError,e:
                  #print e
                  return
     success_times[i] = 1.0
def run():
	  total_success_times = 0
	  for i in range(0,100):
		   thread.start_new_thread(access_cgi,(success_times,i))
	  time.sleep(1.1)
	  for j in range(0,100):
		  total_success_times = total_success_times + success_times[j]

          print total_success_times
if __name__=='__main__':
	run()
