#!/bin/bash

echo -e "\n\n***********************************************************"
echo "              Docker Build Version: $1"
echo -e "***********************************************************\n\n"

cd ..

if [ -d "injectService" ]; then
    rm -rf injectService
fi

if [ -f "ffmpeg" ]; then
    rm -f ffmpeg
fi

cd ..

svn update injectService

cp injectService/ffmpeg/bin/ffmpeg docker/

cd docker

chmod +x ffmpeg
./ffmpeg -version
echo -e "\n"
cd ..

cp injectService docker/ -r

cd docker/injectService/pull

if [ ! -f "/usr/local/lib/libmongoc-1.0.so" ]; then

    echo -e "\ncp libmongoc-1.0.so to /usr/local/lib/"

    cp lib/libmongoc-1.0.so /usr/local/lib/libmongoc-1.0.so
fi

if [ ! -f "/usr/local/lib/libbson-1.0.so" ]; then

    echo "cp libbson-1.0.so to /usr/local/lib/"

    cp lib/libbson-1.0.so /usr/local/lib/libbson-1.0.so
fi

ldconfig

echo -e "\nCompile pull ...\n"

make clean && make

rm -rf inc src makefile
rm -f lib/*.a

cd ../pulld

echo -e "\nCompile pulld ...\n"

make clean && make

rm -rf inc lib src makefile

cd ../..

docker build -t $1 .
