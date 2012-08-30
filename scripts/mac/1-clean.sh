#!/bin/bash

project=`cd "$(dirname $0)/../.."; /bin/pwd`
svn status --no-ignore "$project/trunk" | egrep '^[?I]' | cut -c9- | xargs rm -rv
