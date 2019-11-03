#!/bin/sh

echo --------------------------------------------------------
echo /sys/kernel/debug/dri/0/clients
echo --------------------------------------------------------
sudo cat /sys/kernel/debug/dri/0/clients

echo
echo --------------------------------------------------------
echo /sys/kernel/debug/dri/0/name
echo --------------------------------------------------------
sudo cat /sys/kernel/debug/dri/0/name

echo
echo --------------------------------------------------------
echo /sys/kernel/debug/dri/0/gem_names
echo --------------------------------------------------------
sudo cat /sys/kernel/debug/dri/0/gem_names

