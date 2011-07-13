#!/bin/sh

# Make sure the hwmultd user is in /etc/shutdown.allow
SHUTDOWN="/sbin/shutdown"
SHUTFLAGS="-a -h -k"
PIDFILE="/var/run/shutdown.pid"

KILL="/bin/kill"
TOUCH="/usr/bin/touch"
INPROGRESS="/tmp/hwmultd-shutdown"

DELAY=300

TEMPERATURE=$1

#if you go about the hard maxtemp, start a shutdown if one isn't already in progress
HARD_MAX=35

#if you go below the soft maxtemp, stop a shutdown if its already in progress
SOFT_MAX=30

#same as HARD_MINTEMP, but for a minimum
HARD_MIN=5

#same as SOFT_MAXTEMP, but for a minimum
SOFT_MIN=10


if [[ $TEMPERATURE -gt $HARD_MAX ]]; then
	$SHUTDOWN $SHUTFLAGS $DELAY "Temp above hard max" >/dev/null 2>&1 &
	$TOUCH $INPROGRESS
fi

if [[ $TEMPERATURE -gt $SOFT_MIN && $TEMPERATURE -lt $SOFT_MAX && -e $INPROGRESS ]]; then
	$KILL -TERM $(cat /var/run/shutdown.pid)
	$RM $INPROGRESS
	echo "Temp in normal range --- killing hwmultd initiated shutdown"
fi

if [[ $TEMPERATURE -lt $HARD_MIN ]]; then
	$SHUTDOWN $SHUTFLAGS $DELAY "Temp below hard min" >/dev/null 2>&1 &
	$TOUCH $INPROGRESS
fi

