
一、概述

支持restful API 控制的ffmpeg拉流注入服务。

二、架构

1.单进程多线程多队列架构，严格控制资源的使用，多队列并行exec ffmpeg，可发挥系统最大吞吐量。

2.ffmpeg采用fork形式启动，进程间通信采用rabbitmq。

3.服务是无状态的，消息缓存采用mongodb。

4.restfull 采用libevent http模块。

5.队列消息派发采用访问者模式。

6.log采用log4cplus。

7.守护进程模式下，rest server崩溃后，pulld会间隔5秒拉起，实现高可用。

8.ffmpeg由于拉流不到，或者异常崩溃，都会由schedule监测到并再次拉起ffmpeg，实现高可用。

三、编译

make && make install
