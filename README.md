# CV_Proyecto1

Installing instructions:

Run the following commands from the terminal:

git clone https://github.com/perazad/CV_Proyecto1

cd CV_Proyecto1

make

./proyecto01

Execution Instructions:

This program performs a feature point tracking using an image (-i or -I) or region of interest (-r or -R) - default as baseline.

Region of interest or ROI is taken from first frame in the video or camera. Press Enter after selecting ROI.

The program can run on a video (-v or -V) or using captures from webcam (-c or -C) - default.

The program uses sevaral feature descriptors like: surf (-su or -SU) - default, sift (-si or -SI), akaze (-ak or -AK).

Parameters must be send in order when program is executed.

Press q in image window for exit.

Executions examples:

Running project using a previous image as baseline and capturing from camera using surf algorithm.

./proyecto01 -i cvve.jpeg -c -su

Running project using a region of interest as baseline and capturing from a stored video using sift algorithm

./proyecto01 -r -v test.avi -si

Running project using a region of interest as baseline and capturing from camera using akaze algorithm

./proyecto01 -r -c -ak

