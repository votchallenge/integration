Tracker integration examples
============================

This repository contains several examples of tracker integration for C/C++, Matlab, and Python trackers. Helpers for all languages are provided so that integration is as easy as adding a few lines of code to your tracker source code. For C/C++ three example trackers are provided in the `native` directory, for Matlab an example tracker is in the `matlab` directory, and for Python in the `python` directory. More information can be found in the [documentation](index.md).

C/C++
------------

Three examples are provided by the toolkit. There are two static trackers (trackers that just report the initial position) written in C and C++ and a NCC tracker written in C++ and using OpenCV library.

All three trackers are using `vot.h` header that provides integration functions and classes that can be used to speed up the integration process.

Matlab
------

A NCC tracker example, written in Matlab is provided by the toolkit. The tracker uses `vot` function that generates a communication structure that is then used to communicate with the toolkit.

Python
------

Both static and NCC tracker example, written in Python are provided by the toolkit. The tracker uses `vot.VOT` object that is then used to communicate with the toolkit.
