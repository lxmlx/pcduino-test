#!/bin/bash
# download the image to device

if [ -z $2 ]
then
	echo "Usage $0 *.bin /dev/sd*"
	exit 127
fi

if [ $UID -ne 0 ]
then
	echo "need root permission"
	exit 1
fi

dd if=/dev/zero of=$2 bs=1M count=1
dd if=$1 of=$2 bs=1024 seek=8
