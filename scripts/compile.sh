#!/bin/bash
PKGS="subversion build-essential autotools-dev pkg-config libz-dev libclucene-dev libicu-dev libcurl4-gnutls-dev"
sudo apt-get install $PKGS -y
svn co http://crosswire.org/svn/sword/trunk sword
cd sword
./autogen.sh && ./usrinst.sh --enable-shared && make && sudo make install && sudo make install_config
