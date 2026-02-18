#!/usr/bin/python

import vot
import cv2

from ncc_tracker import NCCTrackerImpl

class NCCTracker(object):
    def __init__(self, image, region):
        self.ncc_ = NCCTrackerImpl(image, region)

    def track(self, image):
        pred_region, max_val = self.ncc_.track(image)
        return self.ncc_._mask_from_rect(pred_region, (image.shape[1], image.shape[0]))

handle = vot.VOT("mask", multiobject=False)
init_mask = handle.region()

imagefile = handle.frame()
image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)

tracker = NCCTracker(image, init_mask)

while True:
    imagefile = handle.frame()
    if not imagefile:
        break
    image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)

    region = tracker.track(image)

    handle.report(region)
