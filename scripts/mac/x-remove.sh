#!/bin/bash

pkg=$1
if [ $# -ne 1 ]; then
    echo "Usage: $0 <package>"
    exit 65
fi

home="/home/frs/project/q/qu/quazaa"
ssh $USER,quazaa@shell.sf.net create
ssh $USER,quazaa@shell.sf.net "cd $home/Debug\ Builds && rm Mac/$pkg && ls -lAR > /home/project-web/quazaa/frs-listing.txt"
ssh $USER,quazaa@shell.sf.net shutdown
