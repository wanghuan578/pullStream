#!/bin/bash

#src_url='"rtmp://devlivepush.migucloud.com/live/DRIKJ087_C0"'

name=10000

index=0
finish=1

start=0
end=1000

for((i=$index;i<$finish;i++)); 
do  
    for((j=$start;j<$end;j++));
    do
	cmd='{"sid":"'"$name"'"}'

	#echo $name

    	sh delete_channel.sh $cmd &
    
    	let name+=1
    done

    sleep 0.5
done
