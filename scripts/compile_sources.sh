#!/usr/bin/env bash
PKGS="subversion build-essential autotools-dev pkg-config libz-dev libclucene-dev libicu-dev libcurl4-gnutls-dev libtool automake"
echo "Installing $PKGS"
apt-get upgrade -y
apt-get update -y
apt-get install $PKGS -y
svn co http://crosswire.org/svn/sword/trunk sword
cd sword && ./autogen.sh && ./usrinst.sh --enable-shared && make && make install && make install_config
