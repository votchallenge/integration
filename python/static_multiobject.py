#!/usr/bin/python

import vot
import cv2

# An example of how to use the toolkit in Python. This is the simplest possible example
# which only reports the initialization region for all frames. It only supports single
# channel (RGB) frames.

handle = vot.VOT("mask", multiobject=True)
objects = handle.objects()

imagefile = handle.frame()

image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)

while True:
    imagefile = handle.frame()
    if not imagefile:
        # Terminate if no new frame was received.
        break
    image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)
    handle.report(objects)

handle.quit()