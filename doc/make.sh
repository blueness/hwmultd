#!/bin/sh

#Run this on developer side, and distribute troff
#in case the end user doesn't have pod2man

rm -f hwmultd.8

pod2man \
 --official \
 --section="8" \
 --release="hwmultd 0.1" \
 --center="SYSTEM ADMINISTRATION COMMANDS" \
 --date="2011-07-19" \
 hwmultd.pod > hwmultd.8
