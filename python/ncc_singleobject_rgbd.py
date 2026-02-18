#!/usr/bin/python

import vot
import sys
import time
import cv2
import numpy
import collections

from ncc_tracker import NCCTrackerImpl

class NCCTracker(object):
    def __init__(self, image, region):
        self.ncc_ = NCCTrackerImpl(image, region)

    def track(self, image):
        pred_region, max_val = self.ncc_.track(image)
        return vot.Rectangle(pred_region[0], pred_region[1], pred_region[2], pred_region[3]), max_val

handle = vot.VOT("rectangle", 'rgbd')
selection = handle.region()

colorimage, depthimage = handle.frame()
if not colorimage:
    sys.exit(0)

image = cv2.imread(colorimage, cv2.IMREAD_GRAYSCALE)
tracker = NCCTracker(image, selection)
while True:
    colorimage, depthimage = handle.frame()
    if not colorimage:
        break
    image = cv2.imread(colorimage, cv2.IMREAD_GRAYSCALE)
    region, confidence = tracker.track(image)
    handle.report(region, confidence)
