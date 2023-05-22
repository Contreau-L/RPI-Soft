#!/bin/bash

# Test if I'm root
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

# Check if the .so is already installed
if [ ! -f /usr/local/lib/aarch64-linux-gnu/libcjson.so ]; then
    git clone https://github.com/DaveGamble/cJSON.git
    cd cJSON
    mkdir -p build
    cd build
    cmake ..
    make
    make install
    cd ../..
fi

make
make install

# Check if LD_LIBRARY_PATH contains /usr/local/lib, if not add it
if ! echo $LD_LIBRARY_PATH | grep -q /usr/local/lib ; then
    echo "LD_LIBRARY_PATH does not contain /usr/local/lib, adding it"
    echo "export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH" >> /home/$SUDO_USER/.bashrc
    source /home/$SUDO_USER/.bashrc
fi

ldconfig
(crontab -l 2>/dev/null; echo "@reboot /usr/local/bin/contreaul > /dev/null") | crontab -

mkdir -p /var/log/contreaul
mkdir -p /etc/contreaul
mkdir -p /var/lib/contreaul

#add right to the user
user=$SUDO_USER

chown $user:$user /var/log/contreaul
chown $user:$user /etc/contreaul
chown $user:$user /var/lib/contreaul

cp contreaulConf.json /etc/contreaul/contreaulConf.json
contreaul
