while true
do
    ffmpeg -re -i apple.mp4 -c copy -f flv rtmp://localhost:1935/live/1
    sleep 1
done
