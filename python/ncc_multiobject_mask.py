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

handle = vot.VOT("mask", multiobject=True)
objects = handle.objects()

imagefile = handle.frame()

image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)

trackers = [NCCTracker(image, object) for object in objects]

while True:
    imagefile = handle.frame()
    if not imagefile:
        break
    image = cv2.imread(imagefile, cv2.IMREAD_GRAYSCALE)
    handle.report([tracker.track(image) for tracker in trackers])
