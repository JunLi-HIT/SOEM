#!/bin/sh

echo "Creating the device"
mknod /dev/rtnet c 10 240;

echo "Unloading linux driver"
rmmod e1000e;

echo "Loading rtnet module"
insmod /usr/local/rtnet/modules/rtnet.ko;

echo "Loading rtipv4 module"
insmod /usr/local/rtnet/modules/rtipv4.ko;

echo "Loading rtpacket module"
insmod /usr/local/rtnet/modules/rtpacket.ko;

echo "Loading rtloopback module"
insmod /usr/local/rtnet/modules/rt_loopback.ko;

echo "Loading rt_e1000e driver"
insmod /usr/local/rtnet/modules/rt_e1000e.ko;

echo "Loading the capture interface"
insmod /usr/local/rtnet/modules/rtcap.ko;

echo "Configuring the loopback"
/usr/local/rtnet/sbin/rtifconfig rtlo up 127.0.0.1
ifconfig rteth0 up
ifconfig rteth0-mac up

echo "Configuring the rteth0 interface"
/usr/local/rtnet/sbin/rtifconfig rteth0 up 10.0.0.2 netmask 255.255.255.0
ifconfig rtlo up


echo "Waiting for 4 seconds ..."
sleep 4;

echo "Attaching to the rtroute"
/usr/local/rtnet/sbin/rtroute add 10.0.0.2 68:05:ca:3a:4a:25 dev rteth0



