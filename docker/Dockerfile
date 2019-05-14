FROM centos:latest

MAINTAINER wanghuan from migu (wanghuan@migu.cn)

WORKDIR /home

RUN yum install openssl-devel java -y

COPY injectService injectService
RUN cd injectService/auto/ && sh setup.sh

RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

ADD ffmpeg /usr/bin/
RUN chmod +x /usr/bin/ffmpeg

ADD entrypoint.sh entrypoint.sh
RUN chmod +x entrypoint.sh

CMD ["./entrypoint.sh"]

EXPOSE 9000
