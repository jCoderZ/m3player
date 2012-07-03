#!/bin/bash

DIR=$(pwd)

rm -rf ${DIR}/install
rm -rf ${DIR}/gupnp

mkdir ${DIR}/install
mkdir ${DIR}/gupnp

cd gupnp

git clone http://git.gnome.org/browse/gssdp
cd gssdp
./autogen.sh --prefix=${DIR}/install --exec-prefix=${DIR}/install
make && make install
cd ..


git clone http://git.gnome.org/browse/gupnp
cd gupnp
PKG_CONFIG_PATH=${DIR}/install/lib/pkgconfig ./autogen.sh --prefix=${DIR}/install --exec-prefix=${DIR}/install
make && make install
cd ..


git clone http://git.gnome.org/browse/gupnp-av
cd gupnp-av
PKG_CONFIG_PATH=${DIR}/install/lib/pkgconfig ./autogen.sh --prefix=${DIR}/install --exec-prefix=${DIR}/install
make && make install
cd ..


git clone http://git.gnome.org/browse/gupnp-tools
cd gupnp-tools
PKG_CONFIG_PATH=${DIR}/install/lib/pkgconfig ./autogen.sh --prefix=${DIR}/install --exec-prefix=${DIR}/install
make && make install
cd ..


cd ..

export PATH=${DIR}/install/bin:${DIR}/gupnp/gupnp/examples:${DIR}/gupnp/gupnp/tools:${DIR}/gupnp/gssdp/tools:$PATH
export LD_LIBRARY_PATH=${DIR}/install/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${DIR}/install/lib/pkgconfig:$PKG_CONFIG_PATH

PKG_CONFIG_PATH=${DIR}/install/lib/pkgconfig ./autogen.sh --prefix=${DIR}/install --exec-prefix=${DIR}/install
make && make install

