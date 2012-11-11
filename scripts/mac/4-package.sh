#!/bin/bash

pwd=`/bin/pwd`
project=`cd "$(dirname $0)/../.."; /bin/pwd`

cd "$project/trunk/Quazaa"
macdeployqt bin/Quazaa_debug.app -use-debug-libs -dmg

revision=`svn info | grep Revision: | cut -c11-`
date=`date "+%Y-%m-%d"`
chmod g+w bin/Quazaa_debug.dmg
mv bin/Quazaa_debug.dmg $pwd/Quazaa_debug-${date}-r${revision}-mac.dmg
echo Quazaa_debug-${date}-r${revision}-mac.dmg
