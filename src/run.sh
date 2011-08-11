#!/bin/sh

export LD_LIBRARY_PATH=/home/micha/workspaces/upnp/gupnp/git/base/lib

rm -f m3player.log

./m3player -c $(pwd)/m3player.ini -x $(pwd) -l $(pwd)/m3player.log $*

