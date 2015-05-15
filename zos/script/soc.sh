#!/bin/sh
[ "$1" ] || { echo "usage: $0 PIPE"; exit 1;}
trap "stty sane" 0 1 2 3 15
socat unix-connect:$1 stdio,raw,echo=0,icanon=0,crnl
