#!/bin/bash

project=`cd "$(dirname $0)/../.."; /bin/pwd`

cd "$project/trunk"
qmake -r -spec macx-g++ -config debug -config x86 -config x86_64 "DEFINES+=_SNAPSHOT_BUILD"
make -j8
