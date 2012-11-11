#!/bin/bash

project=`cd "$(dirname $0)/../.."; /bin/pwd`

cd "$project/trunk"
svn update
