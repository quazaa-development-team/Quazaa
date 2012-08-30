#!/bin/bash

pkg=$1
if [ $# -ne 1 ] || [ ! -f $pkg ]; then
    echo "Usage: $0 <package>"
    exit 65
fi

home="/home/frs/project/q/qu/quazaa"
ssh $USER,quazaa@shell.sf.net create
scp -p $pkg "$USER,quazaa@shell.sf.net:$home/Debug\ Builds/Mac"
ssh $USER,quazaa@shell.sf.net "cd $home/Debug\ Builds && ln -sf Mac/$pkg Quazaa_debug-latest-mac.dmg && ls -lAR > /home/project-web/quazaa/frs-listing.txt"
ssh $USER,quazaa@shell.sf.net shutdown
