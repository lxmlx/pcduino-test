#!/bin/bash
if [ -z $1 ]
then
	echo "Usage: sudo $0 /dev/sdX "
	exit 1
fi

if [ $UID -ne 0 ]
then
	echo "need root permission"
	exit 127
fi

dd if=/dev/zero of=$1 bs=1024 seek=8 count=24
dd if=u-boot-spl.bin of=$1 bs=1024 seek=8
