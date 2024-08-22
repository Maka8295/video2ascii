#!/bin/bash

#Dependencies: mpv bc ffmpeg


# Paths
PROGRAM="./badapple"
IMAGES_FOLDER="./images"
OUTPUT_FOLDER="./txts"


INPUT="$1"
# Check for input file
if [ -z "$1" ]; then
  echo "No input file. Usage: ./badapple.sh input"
  exit 1
fi

# such that we can use this variable for the frame time in play.sh
AUDIO_LEN=$(ffprobe -v error -select_streams a:0 -show_entries stream=duration -of default=noprint_wrappers=1:nokey=1 $INPUT)
echo $AUDIO_LEN > length.txt
echo $INPUT > audio.txt

mkdir ./images
mkdir ./txts

# auto generate images from video using fmpeg
echo "Generating images..."
ffmpeg -i $INPUT -r 30 $IMAGES_FOLDER/img_%04d.bmp > /dev/null 2>&1

echo "Converting images to txt files... This may take a while!"
# Loop through each BMP image in the folder
for IMAGE in "$IMAGES_FOLDER"/*.bmp
do
    # Generate output file name
    BASENAME=$(basename "$IMAGE" .bmp)
    OUTPUT_FILE="$OUTPUT_FOLDER/$BASENAME.txt"
    # Convert the BMP image to ASCII
    $PROGRAM "$IMAGE" "$OUTPUT_FILE"
done
echo "Done!"
