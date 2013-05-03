#!/bin/sh
#tatistic of peer count,res count,report cmd count,list cmd count
case "$1" in "sameroutecmd")
	tail -n 15 /home/logs/natcheck/natcheck_env_*/natcheck_stat.log | grep -A10 dumpinfo | grep reqsameroute | awk '{sum+=$2}; END {print sum}';;
"diffipcmd")
	tail -n 15 /home/logs/natcheck/natcheck_env_*/natcheck_stat.log | grep -A10 dumpinfo | grep reqdiffip | awk '{sum+=$2}; END {print sum}';;
"diffportcmd")
	tail -n 15 /home/logs/natcheck/natcheck_env_*/natcheck_stat.log | grep -A10 dumpinfo | grep reqdiffport | awk '{sum+=$2}; END {print sum}';;
esac
