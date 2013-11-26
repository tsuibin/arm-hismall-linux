#!/bin/sh

for armlinux in ../../bin/arm-hisi-linux-*
do
	ln $armlinux $1`echo $armlinux | cut -b 26-` -sv
done

