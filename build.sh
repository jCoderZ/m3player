#!/bin/bash

DIR=.

mkdir install
mkdir gupnp

cd gupnp

git clone http://git.gnome.org/browse/gssdp
cd gssdp
./autogen.sh --prefix=../../install --exec-prefix=../../install
make && make install
cd ..


git clone http://git.gnome.org/browse/gupnp
cd gupnp
PKG_CONFIG_PATH=../../install/lib/pkgconfig ./autogen.sh --prefix=../../install --exec-prefix=../../install
cd ..


git clone http://git.gnome.org/browse/gupnp-tools
cd gupnp-tools
PKG_CONFIG_PATH=../../install/lib/pkgconfig ./autogen.sh --prefix=../../install --exec-prefix=../../install
cd ..


cd ..

export PATH=./install/bin:./gupnp/gupnp/examples:$PATH
export LD_LIBRARY_PATH=./install/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=./install/lib/pkgconfig:$PKG_CONFIG_PATH

PKG_CONFIG_PATH=install/lib/pkgconfig ./autogen.sh --prefix=install --exec-prefix=install

