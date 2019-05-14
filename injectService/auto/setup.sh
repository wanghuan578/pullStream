#!/bin/bash

echo -e "\nsync .so to /usr/local/lib\n"

cd ../pull/lib

mv *.so /usr/local/lib

echo /usr/local/lib >> /etc/ld.so.conf

ldconfig
