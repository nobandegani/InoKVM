sudo apt update && sudo apt install -y v4l-utils

v4l2-ctl --list-devices

v4l2-ctl --device=/dev/video0 --list-formats-ext

sudo apt update && sudo apt install -y ffmpeg

