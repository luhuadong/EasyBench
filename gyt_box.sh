#!/bin/sh

#export DISPLAY=:0.0
export LD_LIBRARY_PATH=/home/root/seat_imx:$LD_LIBRARY_PATH
cd /home/root/tools
./gyt_box &
