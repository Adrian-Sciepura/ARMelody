#!/bin/bash
APP_BIN="./build/app.elf"
DEBUG_PORT=1234

tmux new-session -d -s qemu_debug "qemu-aarch64 -g $DEBUG_PORT $APP_BIN; bash" 
tmux split-window -h
sleep 1
tmux send-keys "gdb-multiarch $APP_BIN -ex 'target remote :1234' -ex 'b main' -ex 'layout src'" C-m
tmux attach -t qemu_debug
