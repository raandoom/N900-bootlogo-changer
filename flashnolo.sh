#!/bin/sh
softupd --local --standalone &
pid=$!
sleep 1
flasher --local -s /opt/n900-bootlogo-changer/secondary -x /opt/n900-bootlogo-changer/xloader -f
sleep 1
kill $pid
