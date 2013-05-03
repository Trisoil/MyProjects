
from time import localtime,time,strftime
from datetime import datetime
import threading
import xml.dom.minidom
import os
import socket  
import fcntl  
import struct
import smtplib
import config

timer_interval = 5
#from email.mime.text import MIMEText
#############
#mailto_list=["85928733@qq.com","shellexchen@pplive.com"]
mailto_list=["shellexchen@pplive.com"]
#####################

mail_host="smtp.gmail.com:587"

mail_user="ppliveshellexchen"
mail_pass="abcdABCD1234"
mail_postfix="gmail.com"
######################
global server_status
server_status = True
channels_status = {}


def get_ip_address(ifname):  
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  
    return socket.inet_ntoa(fcntl.ioctl(  
        s.fileno(),  
        0x8915, # SIOCGIFADDR  
        struct.pack('256s', ifname[:15])  
    )[20:24]) 

def Init():
    cwd = os.getcwd()
    status_cwd = os.path.join(cwd, "status.xml")
    print status_cwd
    dom = xml.dom.minidom.parse(status_cwd)
    channel_status = dom.getElementsByTagName('channel_status')[0]
    channels = channel_status.getElementsByTagName('channel')
    for channel in channels:
        rid = channel.getAttribute('id')
        channels_status[rid] = True
    

def AnalysisXml():
    current_time = time()
    udpserver_ip = config.server_ip
    print udpserver_ip
    dt_obj = str(datetime.fromtimestamp(time()))
    print current_time
    cwd = os.getcwd()
    status_cwd = os.path.join(cwd, "status.xml")
    print status_cwd
    dom = xml.dom.minidom.parse(status_cwd)
    channel_status = dom.getElementsByTagName('channel_status')[0]
    channels = channel_status.getElementsByTagName('channel')
    um_nomal_channels = {}
    channels_endtime = {}
    max = 0
    min = 100000000000
    min_rid = "1"
    max_rid = "1"
    for channel in channels:
        rid = channel.getAttribute('id')
        end_time = channel.getElementsByTagName('status')[0].getAttribute('end_time')
        #print rid
        #print end_time
        channels_endtime[rid] = end_time
        if channels_status.get(rid) == None:
            channels_status[rid] = True
    
        if time() - float(end_time) > 1200:
            um_nomal_channels[rid] = end_time
        
        if end_time > max:
            max = end_time
            max_rid = rid
        
        if long(end_time) < long(min):
            min = end_time
            min_rid = rid
        
    if int(max) - int(min) > 30:
        min_status = channels_status.get(min_rid)
        if min_status == None or min_status == True:
           channels_status[min_rid] = False
           mail_content = 'server : ' + udpserver_ip + '\r\nrid: ' + min_rid + ' \r\n' + dt_obj
           subject = 'OOPS! ' + udpserver_ip + ' ' + min_rid + ' ' + dt_obj
           send_mail(mailto_list, subject, mail_content) 
    print '***********************************'
    #todo
    for rid, state in channels_status.items():
        print state
        if state == False:
           if current - channels_endtime[rid] < 30:
               channels_status[rid] = True
               mail_content = 'server : ' + udpserver_ip + '\r\nrid: ' + rid + ' \r\n' + dt_obj
               subject = 'OK! ' + udpserver_ip + ' ' + rid + ' ' + dt_obj
               send_mail(mailto_list, subject, mail_content)
        
    if len(um_nomal_channels) > 0:
        global server_status
        if server_status == True:
            server_status = False
            mail_content = 'server : ' + udpserver_ip + 'is imnormal!\r\n channels below are imnormal: \r\n'
            mail_content = mail_content + "current_time: "
            mail_content = mail_content + str(time())
            for rid, end_time in um_nomal_channels.items():
                mail_content = mail_content + rid
                mail_content = mail_content + "  "
                mail_content = mail_content + end_time
                mail_content = mail_content + "  "
                mail_content = mail_content + str(time() - float(end_time))
                mail_content = mail_content + '\r\n\r\n'
            
            #print mail_content
            
            send_mail(mailto_list,"LivemediaWarming: " + udpserver_ip + ' ' + dt_obj, mail_content)
    elif server_status == False:
        mail_content = "server: " + udpserver_ip + "is ok now \r\n"
        send_mail(mailto_list, "LivemediaResume: " + udpserver_ip + ' ' + dt_obj, mail_content)
        server_status = True
        

def send_mail(to_list,sub,content):
    '''
    to_list:发给谁
    sub:主题
    content:内容
    send_mail("aaa@126.com","sub","content")
    '''
    me=mail_user+"<"+mail_user+"@"+mail_postfix+">"
    #msg = MIMEText(content)
    #msg['Subject'] = sub
    #msg['From'] = me
    #msg['To'] = ";".join(to_list)
    msg = ("Subject: %s\r\nFrom: %s\r\nTo: %s \r\n\r\n" % (sub, me, ";".join(to_list)))
    msg += content

    
    try:
        s = smtplib.SMTP()
        #s.esmtp_features['auth'] = 'LOGIN DIGEST-MD5 PLAIN'
        print 'connecting'
        s.connect(mail_host)
        s.set_debuglevel(1)
        s.ehlo()
        print 'connected'
        s.starttls()
        s.ehlo()
        s.login(mail_user,mail_pass)
        s.sendmail(me, to_list, msg)
        s.close()
        return True
    except Exception, e:
        print str(e)
        return False

def OnTimer():
    AnalysisXml()
    global day_timer
    day_timer = threading.Timer(timer_interval,OnTimer)
    day_timer.start()
 
Init()
#AnalysisXml()
day_timer = threading.Timer(timer_interval, OnTimer)
day_timer.start()
