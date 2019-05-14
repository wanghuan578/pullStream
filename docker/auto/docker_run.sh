#!/bin/bash
docker run -d \
-p 9000:9000 \
-v /var/pullStream/pull/log/:/home/pulling/pull/log/ \
-v /var/pullStream/pulld/log/:/home/pulling/pulld/log/ \
-v /var/pullStream/schedule/log/:/var/log/libevtadpt-log/ \
-v /etc/localtime:/etc/localtime \
--net="host" \
$1
