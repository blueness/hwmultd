#!/bin/sh

#Run this on developer side, and distribute troff
#in case the end user doesn't have pod2man
rm -f hwmultd.8
pod2man hwmultd.pod > hwmultd.8
sed -i	-e 's|perl v5.12.3|HWMULTD(1)|' \
	-e 's|User Contributed Perl Documentation|SYSTEM ADMINISTRATION COMMANDS|' \
	hwmultd.8
