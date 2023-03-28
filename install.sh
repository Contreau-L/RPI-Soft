#! /bin/bash


#test if i'm root
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi
#check if the .so is already installed
if [ -f /usr/local/lib/libcjson.so ]; then
    echo "cjson already installed"
    exit 1
fi
git clone https://github.com/DaveGamble/cJSON.git

cd cJSON

mkdir -p build
cd build
cmake ..
make
make install

cd ../..

make
ldconfig
