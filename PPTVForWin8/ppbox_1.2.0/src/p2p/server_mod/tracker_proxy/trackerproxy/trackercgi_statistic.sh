#!/bin/bash
path=`grep log4cplus.appender.log_file.File /usr/local/nginx/conf/cgilogconf.conf|awk -F'=' '{print $2}'`
line=`grep -h sort "$path"*|sort --key=1,2|tail -1|awk '{print $1,$2}'`
NUM=`grep -h -B1 "$line" "$path"*|grep -B1 sort|head -1|awk -F':' '{print $4}'`
NUM2=`expr $NUM - 1`
#echo $ndc
case "$1" in "average_time")
grep -h -A$NUM "$line.*sort" "$path"*|tail -1|awk '{print 1000*$1}';;
"success_ratio")
grep -h -A$NUM2 "$line.*sort" "$path"*|awk '{sum1+=$NF;sum2+=$(NF-1)};END{print sum1*100/sum2}';;
"usability")
./trackercgi_usability_monitor.py;;
esac
