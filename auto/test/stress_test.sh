#!/bin/bash

end=10

./check_file_handle.sh $1

for((i=0;i<$end;i++)); 
do  
    echo "run times: "$i

    sleep 2

    ./multi_create.sh

    sleep 8

    echo -e "\n\nafter ffmpeg create count:"

    sleep 2
    
    ps -ef|grep ffmpeg|wc -l

    echo -e "\n"

    sleep 2

    ./multi_delete.sh

    sleep 2

    echo -e "\n\nafter ffmpeg delete count:"
    
    sleep 2

    ps -ef|grep ffmpeg|wc -l

    ./check_file_handle.sh $1
done

