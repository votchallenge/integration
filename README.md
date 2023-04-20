Tracker integration examples
============================

This repository contains several examples of tracker integration for C/C++, Matlab, and Python trackers. Helpers for all languages are provided so that integration is as easy as adding a few lines of code to your tracker source code. For C/C++ three example trackers are provided in the `native` directory, for Matlab an example tracker is in the `matlab` directory, and for Python in the `python` directory. More information can be found in the [documentation](index.md).

C/C++
------------

Three examples are provided by the toolkit. There are two static trackers (trackers that just report the initial position) written in C and C++ and a NCC tracker written in C++ and using OpenCV library.

All three trackers are using `vot.h` header that provides integration functions and classes that can be used to speed up the integration process. When compiling the tracker, the wrapper expects that `trax.h` is available and that the TraX library is found during tracker runtime.

Python
------

Both static and NCC tracker example, written in Python are provided by the toolkit. 
The tracker uses `vot.VOT` object that is then used to communicate with the toolkit as well as a dedicated wrapper `vot.VOTManager` that handles the tracking loop inernally and can automatically scale
to tracking multiple objects. The wrapper assumes that the trax package is available in Python path. You can install the package using pip: `pip install vot-trax`.

Matlab
------

A NCC tracker example, written in Matlab is provided by the toolkit. The tracker uses `vot` function that generates a communication structure that is then used to communicate with the toolkit. It assumes that `traxserver` MEX file is available somewhere in the Matlab path. 

Note that due to decreasing interest in Matlab as a language of choice in computer vision community, this wrapper currently only supports single object tracking.