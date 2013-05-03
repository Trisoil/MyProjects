#!/bin/sh

lock_file=${HOME}/shell/lock

if [ -e ${lock_file} ];then
	exit 0
fi
touch ${lock_file}
/usr/bin/php -f /home/build/shell/auto_check.php > /home/build/shell/auto_check.log
rm -f ${lock_file}
exit 0
