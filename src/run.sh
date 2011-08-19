#!/bin/sh

export LD_LIBRARY_PATH=/home/micha/workspaces/upnp/gupnp/git/base/lib

rm -f *.log

./m3player -c $(pwd)/m3player.ini -l $(pwd)/m3player.log -s $(pwd) -t /tmp/m3player $*

