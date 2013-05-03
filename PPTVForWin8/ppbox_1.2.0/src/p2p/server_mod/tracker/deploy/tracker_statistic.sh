#!/bin/sh
#statistic of peer count,res count,report cmd count,list cmd count
case "$1" in "peer1")
    tail -n 2 -q  /home/logs/tracker/tracker_env_*/tracker_stat.log  | grep Peer | awk -F"Peer" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"rid1")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Resource | awk -F"Resource" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"report1")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep 0x35 | awk -F"0x35" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"leave1")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep 0x34 | awk -F"0x34" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"list1")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Request | awk -F"Request" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"maxip1")
#    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"Max_ip" '{print $2}' | awk -F"Max RID" '{print $1}' | awk -F: '{print $2,$3}' | awk '{IP[$1]=IP[$1]+$3;}END{for(i in IP){print IP[i]}}' | sort -n -k1 | tail -1;; 
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"Max_ip" '{print $2}' | awk -F"Max RID" '{print $1}' | awk -F: '{print $2,$3}' | awk '{print $3}' | sort -n | tail -1;;
"maxrid1")
#    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"RID" '{print $2}' | awk -F: '{print $2,$3}' | awk '{IP[$1]=IP[$1]+$3;}END{for(i in IP){print IP[i]}}' | sort -n -k1 | tail -1;;
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"RID" '{print $2}' | awk -F: '{print $2,$3}' | awk '{print $3}' | sort -n | tail -1;;
"rate1")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Request | awk -F- '{print $2}' | awk -F: '{print $2, $3}'| awk '{sum1+=$1}; {sum2+=$3}; END{printf "%.2f\n", sum2/sum1}';;
"peer")
    tail -n 2 -q  /home/logs/tracker/tracker_env_*/tracker_stat.log  | grep Peer | awk -F"Peer" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"rid")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Resource | awk -F"Resource" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum}';;    
"report")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep 0x35 | awk -F"0x35" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum*50}';;    
"leave")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep 0x34 | awk -F"0x34" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum*1000}';;    
"list")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Request | awk -F"Request" '{print $2}' | awk '{print $1}' | awk -F: '{sum +=$2}; END{print sum*20}';;    
"maxip")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"Max_ip" '{print $2}' | awk -F"Max RID" '{print $1}' | awk -F: '{print $2,$3}' | awk '{print $3*10}' | sort -n | tail -1;;
"maxrid")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log | grep Max_ip | awk -F"RID" '{print $2}' | awk -F: '{print $2,$3}' | awk '{print $3}' | sort -n | tail -1;;
"rate")
    tail -n 2 -q  /home/logs/tracker/tracker_env*/tracker_stat.log  | grep Request | awk -F- '{print $2}' | awk -F: '{print $2, $3}'| awk '{sum1+=$1}; {sum2+=$3}; END{printf "%d\n", sum2*1000/sum1}';;
"usability")
    trackernum=`ls -l /home/logs/tracker/* | grep tracker_env_ |wc -l`
    #echo $trackernum
    path="/home/pplive/tools/tracker"
    #echo ${path}
    ${path}/tracker_tool 9 $trackernum q;;
esac
