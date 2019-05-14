#!/bin/bash

echo "start pulld"

./injectService/pulld/sbin/pulld

echo "start schedule"

nohup java -jar ./injectService/schedule/target/libevtadpt-1.0.jar &

tailf ./injectService/pulld/log/pid
