
# video2ascii

My final project for Harvard's CS50. This program converts any video into a series of BMP images, converts those into ASCII art and then plays them back in the terminal. 

#### Video Demo: TODO

## How it works

First the bash script badapple.sh creates the neccesary directories and using ffmpeg samples the provided videos into bmp images, with 30 samples being taken a second for 30fps playback.

The script then uses my ascii art generation program written in C to generate a txt file for every image. This program first pulls the metadata out of the header of its input BMP file. Using this metadata we can find where in the file the raw pixel data starts, as well as the images resolution. 

The pixel data is converted into a two dimensional array with each value corresponding to brightness (a colours.c also supports full colour but does not yet support full colour output!). It is then "blurred", by averaging pixel data in a 9x9 square, and then compressed into a smaller 2d array. It then samples the brightness of each pixel and maps it to a corresponding ASCII character, you can easilly swap these out for different characters if you wish!

Finally the play.sh script loads the original audio using mpv, then cycles through every txt file and displays its contents, being careful to time them correctly so that it remains synced with the audio. The frame time can befound from the ammount of txt files generated divided by the length of the video, which is done automatically.

## Dependencies

- mpv
- bc
- ffmpeg

## How to install

Clone this repository by pasting the following into your terminal:

```
git clone https://github.com/Maka8295/video2ascii.git
```
Or you can download simply download (or copy and paste) the project.py file, jobs.py file and the story.py file, make sure they are all in the same directory!

## How to use

First make sure your video file is placed in the same directory as this repository.

Next move to the directory where you cloned my repository.

```
cd path/to/your/directory
```

Now run the script to generate the ASCII and other neccesary files.

```
./badapple.sh your_video
```

Finally execute the play script, you might need to resize the terminal for it to display properly. The video file must stay in the same directory because audio is used from the original file.

```
./play.sh your_video
```

If you want to mute the audio use the following command:

```
./play.sh your_video mute
```

If you want to make an ASCII image for a single input image:

```
./badapple input output
```

Thanks to the CS50 staff! :)
