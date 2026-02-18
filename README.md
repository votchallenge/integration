Tracker integration examples
============================

This repository contains several examples of tracker integration for Python, C/C++ and Matlab trackers. Helpers for all languages are provided so that integration is as easy as adding a few lines of code to your tracker source code. For Python example trackers are provided in the `python` directory, for C/C++ three example trackers are in the `native` directory and for Matlab an example tracker is in the `matlab` directory. More information can be found in the [documentation](index.md).

Python
------

A NCC tracker example implemented in Python is provided to demonstrate the integration of a tracker with the toolkit. Implementation of the tracker is available in the `ncc_tracker.py` file. The following examples are available: 

* **ncc_singleobject.py**: 
A bounding box tracker run on a single object tracking task. 

* **ncc_singleobject_rgbd.py**: 
An example of multi-modal tracking. The main difference compared to `ncc_singleobject.py` is demonstration of how both, the rgb and depth images are obtained during evaluation. This can be used on e.g., VOT-RGBD2020. 

* **ncc_multiobject_mask.py**: 
A segmentation-based tracker, i.e. segmentation mask is used for initialization and to report the tracking output. This example demonstrates also the multi-object tracking (available since VOTS2023). 

* **ncc_singleobject_mask.py**: 
A segmentation-based tracker, i.e. segmentation mask is used for initialization and to report the tracking output. This example demonstrates how a single-target tracker could be run on multi-object dataset, i.e., **sequential mode**. For example: run a single-target tracker on VOTS2023 multi-target dataset.

* **ncc_multiobject_manager.py**: 
This is a similar demo as `ncc_multiobject_mask.py`, just the integration is done slightly more elegant using the VOT manager. 

**Note**: the selection of the demo your integration code should based on, depends on the tracking challenge, dataset specification and your tracker, e.g. single- or multi-object; bounding box or segmentation mask. 

C/C++
------------

Three examples are provided by the toolkit. There are two static trackers (trackers that just report the initial position) written in C and C++ and a NCC tracker written in C++ and using OpenCV library.

All three trackers are using `vot.h` header that provides integration functions and classes that can be used to speed up the integration process. When compiling the tracker, the wrapper expects that `trax.h` is available and that the TraX library is found during tracker runtime.

Matlab
------

A NCC tracker example, written in Matlab is provided by the toolkit. The tracker uses `vot` function that generates a communication structure that is then used to communicate with the toolkit. It assumes that `traxserver` MEX file is available somewhere in the Matlab path. 

Note that due to decreasing interest in Matlab as a language of choice in computer vision community, this wrapper currently only supports single object tracking.