#!/bin/bash

src_url='"rtmp://10.200.41.89:1934/live/1"'

name=10000

index=0
finish=1

start=0
end=1000

dest_url='"rtmp://10.200.41.89:1935/live/'

for((i=$index;i<$finish;i++)); 
do  
    for((j=$start;j<$end;j++));
    do
	cmd='{"dest":'$dest_url''"$name"'","src":'$src_url',"sid":"'"$name"'","type":0}'

	#echo $cmd

    	sh create_channel.sh $cmd &
    
    	let name+=1
    done

    sleep 0.5
done

