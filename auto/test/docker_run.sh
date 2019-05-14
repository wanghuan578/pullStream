#!/bin/bash
sudo docker run -itd \
-p 9001:9000 \
-v /mnt/data/rec_distribute:/mnt/data/rec_distribute/ \
-v /data/rec/log/:/home/rec_distribute/rec/log/ \
-v /data/recd/log/:/home/rec_distribute/recd/log/ \
-v /data/monitor/log/:/var/log/libevtadpt-log/ \
--name=rec-distribute-01 \
rec:1.5 /bin/bash
