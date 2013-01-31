#!/bin/sh

adb push dlmirror_sdk17/libs/armeabi/dlmirror /data/local/tmp/
adb shell "su -c \"/data/local/tmp/dlmirror $@\""
