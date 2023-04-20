/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * This is an example of a stationary tracker. It only reports the initial
 * position for all frames and is used for testing purposes.
 * The main function of this example is to show the developers how to modify
 * their trackers to work with the evaluation environment.
 *
 * Copyright (c) 2015, VOT Committee
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 *
 */

#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdio.h>

#define VOT_MULTI_OBJECT
#define VOT_RECTANGLE
#include "vot.h"

#ifndef TRACKER
#define TRACKER TrackerCSRT
#endif

class Tracker : public VOTTracker {

public:
    Tracker(const VOTImage& image, const VOTRegion& region) : VOTTracker(image, region) {

        tracker = cv::TRACKER::create().dynamicCast<cv::Tracker>();
        cv::Rect initialization;
        initialization << region;
        cv::Mat frame = cv::imread(image.color);
        tracker->init(frame, cv::Rect2d(initialization));

    }

    virtual VOTRegion update(const VOTImage& image) {

        cv::Mat frame = cv::imread(image.color);

        cv::Rect2d rect;

        bool ok = tracker->update(frame, rect);

        if (!ok) {
            std::cout << "Not ok" << std::endl;
        }

        return cv::Rect(rect);

    }

private:

    cv::Ptr<cv::Tracker> tracker;

};


int main( int argc, char** argv) {

    VOTManager<Tracker> vot;

    vot.run();

}

