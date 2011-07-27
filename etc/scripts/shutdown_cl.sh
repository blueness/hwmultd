#!/bin/sh

#	This file is part of hwmultd.
#	Copyright (C) 2011  Anthony G. Basile
#
#	hwmultd is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	hwmultd is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


#Email: DOMAIL=1 enabled, else disabled
DOMAIL=1
FROMEMAIL="root@opensource.dyc.edu"
TOEMAIL="basile@opensource.dyc.edu"
SUBJECT="HWMULTD: temperature alert"

MAIL="/bin/mail"
ECHO="/bin/echo"

# Make sure the hwmultd user is in /etc/shutdown.allow
SHUTDOWN="/sbin/shutdown"
SHUTFLAGS="-a -h"
KILL="/bin/kill"
CAT="/bin/cat"
PIDFILE="/var/run/shutdown.pid"

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
	echo -n "CRIT: Temp hot"
	if [ ! -e $INPROGRESS ]; then
		echo -n " shutdown initiated"
		if [ $DOMAIL -eq 1 ]; then
			$ECHO -e "Temperature $TEMPERATURE above hard max\n" \
				"Shutdown initiated with delay $DELAY minutes\n" | \
				$MAIL -s "$SUBJECT" $TOEMAIL
		fi
		$SHUTDOWN $SHUTFLAGS $DELAY "Temp above hard max" >/dev/null 2>&1 <&- &
		$TOUCH $INPROGRESS
	fi
fi

if [ $TEMPERATURE -ge $SOFT_MAX -a $TEMPERATURE -le $HARD_MAX ]; then
	echo -n "WARN: Temp hot"
	if [ -e $INPROGRESS ]; then
		echo -n " proceeding with shtudown"
	fi
fi

if [ $TEMPERATURE -gt $SOFT_MIN -a $TEMPERATURE -lt $SOFT_MAX ]; then
	echo -n "INFO: Temp normal"
	if [ -e $INPROGRESS ]; then
		echo -n " shtudown aborted"
		if [ $DOMAIL -eq 1 ]; then
			$ECHO -e "Temperature $TEMPERATURE within acceptable range\n" \
				"Shutdown terminated\n" | \
				$MAIL -s "$SUBJECT" $TOEMAIL
		fi
		if [ -e $PIDFILE ]; then
			$KILL $($CAT $PIDFILE) >/dev/null 2>&1 <&- &
		fi
		$RM $INPROGRESS
	fi
fi

if [ $TEMPERATURE -le $SOFT_MIN -a $TEMPERATURE -ge $HARD_MIN ]; then
	echo -n "WARN: Temp cold"
	if [ -e $INPROGRESS ]; then
		echo -n " proceeding with shtudown"
	fi
fi

if [ $TEMPERATURE -lt $HARD_MIN ]; then
	echo -n "CRIT: Temp cold"
	if [ ! -e $INPROGRESS ]; then
		echo -n " shutdown initiated"
		if [ $DOMAIL -eq 1 ]; then
			$ECHO -e "Temperature $TEMPERATURE below hard min\n" \
				"Shutdown initiated with delay $DELAY minutes\n" | \
				$MAIL -s "$SUBJECT" $TOEMAIL
		fi
		$SHUTDOWN $SHUTFLAGS $DELAY "Temp below hard min" >/dev/null 2>&1 &
		$TOUCH $INPROGRESS
	fi
fi
