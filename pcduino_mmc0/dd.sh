#!/bin/bash
if [ -z $2 ]
then
	echo "Usage: sudo $0 *.bin /dev/sdX "
	exit 1
fi

if [ $UID -ne 0 ]
then
	echo "need root permission"
	exit 127
fi

dd if=/dev/zero of=$2 bs=1024 count=32
dd if=$1 of=$2 bs=1024 seek=8
