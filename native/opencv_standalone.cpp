
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>

cv::Rect2d read_initailization() {

  std::ifstream infile("groundtruth.txt");

  if (!infile.good()) return cv::Rect2d();
    std::string line, s;
    std::getline(infile, line);

    std::stringstream ss(line);
    std::vector<float> init;

    while (getline(ss, s, ','))
        init.push_back(std::stof(s));

    return cv::Rect2d(init[0], init[1], init[2], init[3]);

}

int main( int argc, char** argv) {

    cv::Ptr<cv::Tracker> tracker = cv::TrackerCSRT::create();

    int i = 1;

    cv::Rect2d initialization = read_initailization();
    cv::Mat image = cv::imread(cv::format("%08d.jpg", i));
    tracker->init(image, initialization);

    while (true) {

        i++;

        cv::Mat image = cv::imread(cv::format("%08d.jpg", i));

        if (image.empty()) break;

        cv::Rect2d rect;

        bool ok = tracker->update(image, rect);

        if (ok)
            cv::rectangle(image, cv::Point(rect.tl()), cv::Point(rect.br()), cv::Scalar(0, 0, 255), 3);

        cv::imshow("Tracker", image);
        cv::waitKey(30);

    }

}

