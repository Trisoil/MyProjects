#!/bin/bash

cd `dirname $0`
program_file='SuperNodeService-linux-x64-gcc44-mt'
monitor_file='ServiceHealth.txt'

function terminate
{
    /bin/kill -TERM "$1" > /dev/null 2>&1
    ret=$?

    if [ $ret -eq 0 ]; then
        TERM_TIMEOUT=60

        while [ $TERM_TIMEOUT -gt 0 ]; do
            /bin/kill -0 "$1" > /dev/null 2>&1 || break
            sleep 1
            let TERM_TIMEOUT=${TERM_TIMEOUT}-1

            echo "waiting ..."
        done

        if [ $TERM_TIMEOUT -eq 0 ]; then
            echo "the SN-server can't terminate normally, kill it"
            /bin/kill -KILL "$1"
        fi
    fi

    return $ret
}

function stop
{
    for pid in `ps aux | grep "$program_file" | grep -v grep | awk "{print $2}"`; do
        terminate $pid
    done
}

function start
{
    ./$program_file --background 1>/dev/null 2>&1 &
}

function check_health
{
    rm -f "$monitor_file"

    MONITOR_FILE_UPDATE_PERIOD=5
    let TIMEOUT=${MONITOR_FILE_UPDATE_PERIOD}*2

    while [ $TIMEOUT -gt 0 ]; do
        if [ -f "$monitor_file" ]; then
            read < "$monitor_file" health_status
            [ "$health_status" = "0" ]
            return $?
        else
            echo "waiting ..."
            sleep 1
            let TIMEOUT=${TIMEOUT}-1
        fi
    done

    return 1
}

function update
{
    if ! check_health; then
        echo 'the SN-server is not started or in bad state, restart it ...'
        stop
        start
    else
        echo 'the SN-server is in health state'
    fi
}

case "$1" in
    start)
        stop
        start
        ;;
    stop)
        stop
        ;;
    update)
        update
        ;;
    restart)
        stop
        start
        ;;
    *)
        echo "Usage: $0 {start|stop|restart|update}"
        exit 1
esac
