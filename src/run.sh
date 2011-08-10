#!/bin/sh

export LD_LIBRARY_PATH=/home/micha/workspaces/upnp/gupnp/git/base/lib

./m3player -c $(pwd)/m3player.ini -x $(pwd) $*

