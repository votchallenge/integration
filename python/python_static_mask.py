#!/usr/bin/python

import vot
import sys
import time

# *****************************************
# VOT: Create VOT handle at the beginning
#      Then get the initializaton region
#      and the first image
# *****************************************
handle = vot.VOT("mask")
selection = handle.region()

# Process the first frame
image = handle.frame()
if not image:
    sys.exit(0)

while True:
    # *****************************************
    # VOT: Call frame method to get path of the
    #      current image frame. If the result is
    #      null, the sequence is over.
    # *****************************************
    image = handle.frame()
    if not image:
        break

    # *****************************************
    # VOT: Report the position of the object
    #      every frame using report method.
    # *****************************************
    handle.report(selection)

    time.sleep(0.01)

