#!/usr/bin/python

# This is a simple example of a tracker implemented in Python. It uses OpenCV to compute the normalized cross correlation and find the best match with a template from a first frame.
# The demo is implemented using VOT Manager to show how a single object tracker can be quickly adapted to multi object scenarios.

import vot
import cv2

def NCCTracker(image, region):
        
    image = cv2.imread(image, cv2.IMREAD_GRAYSCALE)
    window = max(region.width, region.height) * 2

    left = max(region.x, 0)
    top = max(region.y, 0)

    right = min(region.x + region.width, image.shape[1] - 1)
    bottom = min(region.y + region.height, image.shape[0] - 1)

    template = image[int(top):int(bottom), int(left):int(right)]
    position = [region.x + region.width / 2, region.y + region.height / 2]
    size = (region.width, region.height)

    def track(image):
        image = cv2.imread(image, cv2.IMREAD_GRAYSCALE)

        left = max(round(position[0] - float(window) / 2), 0)
        top = max(round(position[1] - float(window) / 2), 0)

        right = min(round(position[0] + float(window) / 2), image.shape[1] - 1)
        bottom = min(round(position[1] + float(window) / 2), image.shape[0] - 1)

        if right - left < template.shape[1] or bottom - top < template.shape[0]:
            return vot.Rectangle(position[0] + size[0] / 2, position[1] + size[1] / 2, size[0], size[1])

        cut = image[int(top):int(bottom), int(left):int(right)]

        matches = cv2.matchTemplate(cut, template, cv2.TM_CCOEFF_NORMED)
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(matches)

        position[0] = left + max_loc[0] + float(size[0]) / 2
        position[1] = top + max_loc[1] + float(size[1]) / 2

        # An example of how to use the response value threshold to report object not found
        if max_val < 0.5:
            return vot.Empty()

        return vot.Rectangle(left + max_loc[0], top + max_loc[1], size[0], size[1])

    return track

if __name__ == "__main__":
    print(vot.__file__)
    manager = vot.VOTManager(NCCTracker, "rectangle")
    manager.run()
