#!/bin/sh
#statistic of peer count,res count,report cmd count,list cmd count
case "$1" in "recv")
	tail -n 1 -q  /home/logs/push/push_env_*/pushstatistics.log | awk -F"recv" '{print $2}' | awk '{sum+=$4};END{print sum}';;
"tasks")
	tail -n 1 -q  /home/logs/push/push_env_*/pushstatistics.log | awk -F"total push" '{print $2}' | awk '{sum+=$4};END{print sum}';;
"notask")
	tail -n 1 -q  /home/logs/push/push_env_*/pushstatistics.log | awk -F"no task" '{print $2}' | awk '{sum+=$4};END{print sum}';;
"pasv")
	tail -n 1 -q  /home/logs/push/push_env_*/pushstatistics.log | awk -F"passive push" '{print $2}' | awk '{sum+=$4};END{print sum}';;
"active")
	tail -n 1 -q  /home/logs/push/push_env_*/pushstatistics.log | awk -F"active push" '{print $2}' | awk '{sum+=$4};END{print sum}';;
"usability")
	pushnum=`ls -l /usr/local/push/* | grep push_env_ |wc -l`
#	echo $pushnum
	path="/home/pplive/tools/push"
#	echo $path
	${path}/tool_push-linux-x86-gcc44-mt $pushnum;;
esac
