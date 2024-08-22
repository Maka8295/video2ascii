#!/bin/bash

TXT_FILES="./txts"
FILE_NUM=$(ls -1 $TXT_FILES | wc -l)
AUDIO_LEN=$(cat length.txt)
INPUT=$(cat audio.txt)

# should be about 0.0334
FRAME_RATE=$(echo "scale=2; $AUDIO_LEN / $FILE_NUM" + 0.001| bc)

ARG="$1"

if [ "$ARG" != "mute" ]; then
    mpv --no-video "$INPUT" > /dev/null 2>&1 &
fi

for TXT in "$TXT_FILES"/*.txt
do
    cat "$TXT"
    sleep $FRAME_RATE
    clear
done
