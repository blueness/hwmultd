#!/bin/sh

# Make sure the hwmultd user is in /etc/shutdown.allow
SHUTDOWN="/sbin/shutdown"
SHUTFLAGS="-a -h"
SHUTCANCEL="-c"

TOUCH="/usr/bin/touch"
RM="/bin/rm"
INPROGRESS="/tmp/hwmultd-shutdown"

TEMPERATURE=$1

#how many minutes to delay before actual shutdown
DELAY=30

#if you go about the hard maxtemp, start a shutdown if one isn't already in progress
HARD_MAX=35

#if you go below the soft maxtemp, stop a shutdown if its already in progress
SOFT_MAX=30

#same as HARD_MINTEMP, but for a minimum
HARD_MIN=5

#same as SOFT_MAXTEMP, but for a minimum
SOFT_MIN=10


if [ $TEMPERATURE -gt $HARD_MAX ]; then
	echo -n "Temp hot"
	$SHUTDOWN $SHUTFLAGS $DELAY "Temp above hard max" >/dev/null 2>&1 <&- &
	$TOUCH $INPROGRESS
fi

if [ $TEMPERATURE -gt $SOFT_MIN -a $TEMPERATURE -lt $SOFT_MAX ]; then
	echo -n "Temp normal"
	if [ -e $INPROGRESS ]; then
		$RM $INPROGRESS
		$SHUTDOWN $SHUTCANCEL >/dev/null 2>&1 <&- &
	fi
fi

if [ $TEMPERATURE -lt $HARD_MIN ]; then
	echo -n "Temp cold"
	$SHUTDOWN $SHUTFLAGS $DELAY "Temp below hard min" >/dev/null 2>&1 &
	$TOUCH $INPROGRESS
fi
