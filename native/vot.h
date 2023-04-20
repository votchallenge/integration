/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * This header file contains C functions that can be used to quickly integrate
 * VOT challenge support into your C or C++ tracker.
 *
 * Copyright (c) 2023, VOT Initiative
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
 */

#ifndef _VOT_TOOLKIT_H
#define _VOT_TOOLKIT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <trax.h>

#define VOT_READ_BUFFER 2024
#define VOT_MAX_OBJECTS 100

// Define VOT_OPENCV after including OpenCV core header to enable better OpenCV support
#if defined(__OPENCV_CORE_HPP__) || defined(OPENCV_CORE_HPP)
#  define VOT_OPENCV
#endif

#ifdef VOT_RECTANGLE
    #ifndef _VOT_REGION
        #define _VOT_REGION 1
    #endif
#elif defined(VOT_POLYGON)
    #ifndef _VOT_REGION
        #define _VOT_REGION 2
    #endif
#elif defined(VOT_MASK)
    #ifndef _VOT_REGION
        #define _VOT_REGION 3
    #endif
#else
    #define _VOT_REGION 1
#endif

// Default region is polygon
#ifndef _VOT_REGION
    #define _VOT_REGION 2
#endif

typedef struct vot_image {
#ifdef VOT_RGBD
    char color[VOT_READ_BUFFER];
    char depth[VOT_READ_BUFFER];
#elif defined(VOT_IR)
    char ir[VOT_READ_BUFFER];
#elif defined(VOT_RGBT)
    char color[VOT_READ_BUFFER];
    char ir[VOT_READ_BUFFER];
#else
    char color[VOT_READ_BUFFER];
#endif
} vot_image;

#if _VOT_REGION == 1

typedef struct vot_region {
    float x;
    float y;
    float width;
    float height;
} vot_region;

void vot_region_release(vot_region** region) {

    if (!(*region)) return;

    free(*region);

    *region = NULL;

}

vot_region* vot_region_create() {
    vot_region* region = (vot_region*) malloc(sizeof(vot_region));
    region->x = 0;
    region->y = 0;
    region->width = 0;
    region->height = 0;
    return region;
}

vot_region* vot_region_copy(const vot_region* region) {
    vot_region* copy = vot_region_create();
    copy->x = region->x;
    copy->y = region->y;
    copy->width = region->width;
    copy->height = region->height;
    return copy;
}

#endif
#if _VOT_REGION == 2
typedef struct vot_region {
    float* x;
    float* y;
    int count;
} vot_region;

void vot_region_release(vot_region** region) {
    if (!(*region)) return;

    if ((*region)->x) {
        free((*region)->x);
        (*region)->x = NULL;
    }
    if ((*region)->y) {
        free((*region)->y);
        (*region)->y = NULL;
    }

    free(*region);

    *region = NULL;
}

vot_region* vot_region_create(int n) {
    vot_region* region = (vot_region*) malloc(sizeof(vot_region));
    region->x = (float *) malloc(sizeof(float) * n);
    region->y = (float *) malloc(sizeof(float) * n);
    memset(region->x, 0, sizeof(float) * n);
    memset(region->y, 0, sizeof(float) * n);
    region->count = n;
    return region;
}

vot_region* vot_region_copy(const vot_region* region) {
    vot_region* copy = vot_region_create(region->count);
    int i;
    for (i = 0; i < region->count; i++) {
        copy->x[i] = region->x[i];
        copy->y[i] = region->y[i];
    }
    return copy;
}
#endif

#if _VOT_REGION == 3
typedef struct vot_region {
    int width;
    int height;
    char* data;
} vot_region;

void vot_region_release(vot_region** region) {

    if (!(*region)) return;
    free((*region)->data);
    free(*region);
    *region = NULL;

}

vot_region* vot_region_create(int width, int height) {
    vot_region* region = (vot_region*) malloc(sizeof(vot_region));
    region->width = width;
    region->height = height;
    region->data = (char*) malloc(sizeof(char) * width * height);
    memset(region->data, sizeof(char) * width * height, 0);
    return region;
}

vot_region* vot_region_copy(const vot_region* region) {
    vot_region* copy = vot_region_create(region->width, region->height);
    memcpy(copy->data, region->data, region->width * region->height);
    return copy;
}


#endif

typedef vot_region** vot_objects;

#ifdef __cplusplus

#include <string>
#include <fstream>
#include <iostream>
#include <type_traits>

using namespace std;

class VOT;

typedef struct VOTImage {
#ifdef VOT_RGBD
    string color;
    string depth;
#elif defined(VOT_IR)
    string ir;
#elif defined(VOT_RGBT)
    string color;
    string ir;
#else
    string color;
#endif
} VOTImage;

class VOTRegion {
    friend class VOT;
public:

    ~VOTRegion() {
        vot_region_release(&_region);
    }

    VOTRegion(const vot_region* region) {
        _region = vot_region_copy(region);
    }

    VOTRegion(const VOTRegion& region) {
        _region = vot_region_copy(region._region);
    }

    VOTRegion(VOTRegion&& region) {
        _region = region._region;
        region._region = NULL;
    }

    VOTRegion& operator= (VOTRegion&& region) {
        vot_region_release(&_region);
        _region = region._region;
        region._region = NULL;
        return *this;
    }

#if _VOT_REGION == 1

    VOTRegion() {
        _region = vot_region_create();
    }

    float get_x() const { return _region->x; }
    float get_y() const { return _region->y; }
    float get_width() const { return _region->width; }
    float get_height() const { return _region->height; }

    float set_x(float x) { return _region->x = x; }
    float set_y(float y) { return _region->y = y; }
    float set_width(float width) { return _region->width = width; }
    float set_height(float height) { return _region->height = height; }

#endif
#if _VOT_REGION == 2
    VOTRegion(int count) {
        _region = vot_region_create(count);
    }

    void set(int i, float x, float y) { assert(i >= 0 && i < _region->count); _region->x[i] = x; _region->y[i] = y; }
    float get_x(int i) const { assert(i >= 0 && i < _region->count); return _region->x[i]; }
    float get_y(int i) const { assert(i >= 0 && i < _region->count); return _region->y[i]; }
    int count() const { return _region->count; }
#endif
#if _VOT_REGION == 3
    VOTRegion(int width, int height) {
        _region = vot_region_create(width, height);
    }

    int width() const { return _region->width; }
    int height() const { return _region->height; }
    char get(int x, int y) const { return _region->data[x + y * _region->width]; }
    void set(int x, int y, char val) { _region->data[x + y * _region->width] = val; }

#endif


    VOTRegion& operator= (const VOTRegion &source) {

        if (this == &source)
            return *this;

#if _VOT_REGION == 1

        set_x(source.get_x());
        set_y(source.get_y());
        set_width(source.get_width());
        set_height(source.get_height());

#endif
#if _VOT_REGION == 2

        if (this->_region->count != source.count()) {
            vot_region_release(&(this->_region));
            this->_region = vot_region_create(source.count());
        }

        for (int i = 0; i < source.count(); i++) {
            set(i, source.get_x(i), source.get_y(i));
        }

#endif
#if _VOT_REGION == 3

        if (this->_region->width != source.width() || this->_region->height != source.height()) {
            vot_region_release(&(this->_region));
            this->_region = vot_region_create(source.width(), source.height());
        }

        memcpy(this->_region->data, source._region->data, this->_region->height * this->_region->width);

#endif

        return *this;
    }

#ifdef VOT_OPENCV
#if _VOT_REGION == 1

    VOTRegion(const cv::Rect& rectangle) {
        _region = vot_region_create();
        set(rectangle);
    }

    void set(const cv::Rect& rectangle) {

        set_x(rectangle.x);
        set_y(rectangle.y);
        set_width(rectangle.width);
        set_height(rectangle.height);

    }

    void get(cv::Rect& rectangle) const {

        rectangle.x = get_x();
        rectangle.y = get_y();
        rectangle.width = get_width();
        rectangle.height = get_height();

    }

    void operator= (cv::Rect& rectangle) {
        this->get(rectangle);
    }

#endif
#if _VOT_REGION == 2

    VOTRegion(const cv::Rect& rectangle) {
        _region = vot_region_create(4);
        set(rectangle);
    }

    void set(const cv::Rect& rectangle) {

        if (_region->count != 4) {
            vot_region_release(&(this->_region));
            _region = vot_region_create(4);
        }

	    set(0, rectangle.x, rectangle.y);
	    set(1, rectangle.x + rectangle.width, rectangle.y);
	    set(2, rectangle.x + rectangle.width, rectangle.y + rectangle.height);
	    set(3, rectangle.x, rectangle.y + rectangle.height);
    }

    void get(cv::Rect& rectangle) const {

	    float top = FLT_MAX;
	    float bottom = FLT_MIN;
	    float left = FLT_MAX;
	    float right = FLT_MIN;

	    for (int j = 0; j < _region->count; j++) {
		    top = MIN(top, _region->y[j]);
		    bottom = MAX(bottom, _region->y[j]);
		    left = MIN(left, _region->x[j]);
		    right = MAX(right, _region->x[j]);
	    }

        rectangle.x = left;
        rectangle.y = top;
        rectangle.width = right - left;
        rectangle.height = bottom - top;

    }

    void operator= (cv::Rect& rectangle) {
        this->get(rectangle);
    }


#endif
#if _VOT_REGION == 3

    VOTRegion(const cv::Mat& mask) {
        assert(!mask.empty() && mask.channels() == 2 && mask.elemSize() == 1);

        _region = vot_region_create(mask.cols, mask.rows);
        set(mask);
    }

    void set(const cv::Mat& mask) {

        assert(!mask.empty() && mask.channels() == 2 && mask.elemSize() == 1);

        if (_region->width != mask.cols || _region->height != mask.rows) {
            vot_region_release(&(this->_region));
            _region = vot_region_create(mask.cols, mask.rows);
        }

        for (int i = 0; i < mask.rows; i++) {
            memcpy(&(_region->data[mask.cols * i]), mask.prt(i), _region->width * sizeof(char));
        }

    }

    void get(cv::Mat& mask) const {

	    mask.create(cv::Size(_region->height, _region->width), CV_8UC1);

        memcpy(mask.prt(0), _region->data, _region->height * _region->width * sizeof(char));

    }

    void operator= (cv::Mat& mask) {
        this->get(mask);
    }

#endif
#endif

protected:

    vot_region* _region;

};

#ifdef VOT_OPENCV
#if _VOT_REGION == 1 || _VOT_REGION == 2
void operator<< (VOTRegion &source, const cv::Rect& rectangle) {
    source.set(rectangle);
}

void operator>> (const VOTRegion &source, cv::Rect& rectangle) {
    source.get(rectangle);
}

void operator<< (cv::Rect& rectangle, const VOTRegion &source) {
    source.get(rectangle);
}

void operator>> (const cv::Rect& rectangle, VOTRegion &source) {
    source.set(rectangle);
}
#endif
#if _VOT_REGION == 3
void operator<< (VOTRegion &source, const cv::Mat& mask) {
    source.set(mask);
}

void operator>> (const VOTRegion &source, cv::Mat& mask) {
    source.get(mask);
}

void operator<< (cv::Mat& mask, const VOTRegion &source) {
    source.get(mask);
}

void operator>> (const cv::Mat& mask, VOTRegion &source) {
    source.set(mask);
}

#endif
#endif

class VOT {
public:
    VOT() {
        vot_initialize();
    }

    ~VOT() {
        vot_quit();
    }

    #ifdef VOT_MULTI_OBJECT
    const std::vector<VOTRegion> objects() {

        std::vector<VOTRegion> wrappers;
        
        for (int i = 0; i < VOT_MAX_OBJECTS; i++) {
            if (_objects[i])
                wrappers.push_back(VOTRegion(_objects[i]));
            else break;
        }

        return wrappers;
    }

    void report(const std::vector<VOTRegion>& objects) {

        vot_objects unwrapped = new vot_region*[objects.size()+1];

        for (int i = 0; i < objects.size(); i++) {
            unwrapped[i] = objects[i]._region;
        }

        unwrapped[objects.size()] = NULL;

        vot_report(unwrapped);
    }

    #else
    const VOTRegion region() {
        return VOTRegion(_objects[0]);
    }

    void report(const VOTRegion& region, float confidence = 1) {
        vot_report2(region._region, confidence);
    }
    #endif

#if defined(VOT_RGBD) || defined(VOT_RGBT)
    const VOTImage frame() {
        return image();
    }
#else
    const string frame() {
        return image().color;
    }
#endif

    const VOTImage image() {

        const vot_image* result = vot_frame();
        VOTImage wrapper;

        if (!result)
            return wrapper;

#if defined(VOT_RGBD)
        wrapper.color = string(_image.color);
        wrapper.depth = string(_image.depth);
#elif defined(VOT_RGBT)
        wrapper.color = string(_image.color);
        wrapper.ir = string(_image.ir);
#elif defined(VOT_IR)
        wrapper.ir = string(_image.ir);
#else
        wrapper.color = string(_image.color);
#endif
        return wrapper;
    }

    bool end() {
        return vot_end() != 0;
    }

private:

    void vot_quit();

    const vot_image* vot_frame();

#ifdef VOT_MULTI_OBJECT
    void vot_report(vot_objects objects);
    vot_objects vot_initialize();
#else
    vot_region* vot_initialize();
    void vot_report(vot_region* region);
    void vot_report2(vot_region* region, float confidence);
#endif

    int vot_end();

#endif

    // Current position in the sequence
    int _vot_sequence_position;
    // Size of the sequence
    int _vot_sequence_size;
    // List of image file names
    char** _vot_sequence;
    // List of results
    vot_region** _vot_result;

    trax_handle* _trax_handle;

    vot_image _image;

    vot_region* _objects[VOT_MAX_OBJECTS];

#if _VOT_REGION == 1

vot_region* _trax_to_region(const trax_region* _trax_region) {
    vot_region* region = vot_region_create();
    assert(trax_region_get_type(_trax_region) == TRAX_REGION_RECTANGLE);
    trax_region_get_rectangle(_trax_region, &(region->x), &(region->y), &(region->width), &(region->height));
    return region;
}
trax_region* _region_to_trax(const vot_region* region) {
    return trax_region_create_rectangle(region->x, region->y, region->width, region->height);
}

#endif

#if _VOT_REGION == 2

vot_region* _trax_to_region(const trax_region* _trax_region) {
    int i;
    int count = trax_region_get_polygon_count(_trax_region);
    vot_region* region = vot_region_create(count);
    for (i = 0; i < count; i++)
        trax_region_get_polygon_point(_trax_region, i, &(region->x[i]), &(region->y[i]));
    return region;
}
trax_region* _region_to_trax(const vot_region* region) {
    int i;
    trax_region* _trax_region = trax_region_create_polygon(region->count);
    assert(trax_region_get_type(_trax_region) == TRAX_REGION_POLYGON);
    for (i = 0; i < region->count; i++)
        trax_region_set_polygon_point(_trax_region, i, region->x[i], region->y[i]);
    return _trax_region;
}
#endif

#if _VOT_REGION == 3

vot_region* _trax_to_region(const trax_region* _trax_region) {
    int x, y, width, height, i;
    trax_region_get_mask_header(_trax_region, &x, &y, &width, &height);
    vot_region* region = vot_region_create(x + width, y + height);

    for (i = 0; i < height; i++) {
        memcpy(&(region->data[(width + x) * (i + y) + x]), trax_region_get_mask_row(_trax_region, i), width * sizeof(char));
    }

    return region;
}

trax_region* _region_to_trax(const vot_region* region) {

    trax_region* _trax_region = trax_region_create_mask(0, 0, region->width, region->height);
    memcpy(trax_region_write_mask_row(_trax_region, 0), region->data, region->width * region->height * sizeof(char));
    return _trax_region;

}
#endif


#ifdef __cplusplus

};

#ifdef VOT_MULTI_OBJECT

class VOTTracker {

public:

    VOTTracker(const VOTImage& image, const VOTRegion& region) { }

    virtual VOTRegion update(const VOTImage& image) = 0;

};

template<typename T>
class VOTManager {
    
public:

    VOTManager() {
        _vot = new VOT();
    }

    ~VOTManager() {
        if (_vot)
            delete _vot;
    }

    void run() {

        std::vector<VOTRegion> objects = _vot->objects();
        VOTImage image = _vot->image();

        for (int i = 0; i < objects.size(); i++) {
            _trackers.push_back(new T(image, objects[i]));
        }

        while (!_vot->end()) {

            VOTImage image = _vot->image();
            std::vector<VOTRegion> state;

            for (int i = 0; i < _trackers.size(); i++) {
                state.push_back(_trackers[i]->update(image));
            }

            _vot->report(state);

        }

        for (int i = 0; i < _trackers.size(); i++) {
            delete _trackers[i];
        }

    }

private:

    VOT* _vot = NULL;

    std::vector<T*> _trackers;

};

#endif

#endif

#ifdef __cplusplus
#  define VOT_PREFIX(FUN) VOT::FUN
#  define VOT_WRAPPER "cpp"
#else
#  define VOT_PREFIX(FUN) FUN
#  define VOT_WRAPPER "c"
#endif


/**
 * Reads the input data and initializes all structures. Returns the initial
 * position of the object as specified in the input data. This function should
 * be called at the beginning of the program.
 */
#ifdef VOT_MULTI_OBJECT
vot_objects
#else
vot_region* 
#endif
VOT_PREFIX(vot_initialize)() {

    int j;
    int flags;
    FILE *inputfile;
    FILE *imagesfile;

    _vot_sequence_position = 0;
    _vot_sequence_size = 0;
    flags = 0;

    #ifdef VOT_MULTI_OBJECT
    flags |= TRAX_METADATA_MULTI_OBJECT;
    #endif

    assert(_trax_handle == NULL);

    trax_configuration config;
    trax_image_list* _trax_image = NULL;
    trax_object_list* _trax_objects = NULL;
    _trax_handle = NULL;
    int response;
    #if _VOT_REGION == 1
    int region_format = TRAX_REGION_RECTANGLE;
    #elif _VOT_REGION == 2
    int region_format = TRAX_REGION_POLYGON;
    #else
    int region_format = TRAX_REGION_MASK;
    #endif

    #ifdef VOT_RGBD
    int channels = TRAX_CHANNEL_COLOR | TRAX_CHANNEL_DEPTH;
    #elif defined(VOT_IR)
    int channels = TRAX_CHANNEL_IR;
    #elif defined(VOT_RGBT)
    int channels = TRAX_CHANNEL_COLOR | TRAX_CHANNEL_IR;
    #else
    int channels = TRAX_CHANNEL_COLOR;
    #endif

    trax_metadata* metadata = trax_metadata_create(region_format, TRAX_IMAGE_PATH, channels, NULL, NULL, NULL, flags);

    trax_properties_set(metadata->custom, "vot", VOT_WRAPPER);

    _trax_handle = trax_server_setup(metadata, trax_no_log);

    trax_metadata_release(&metadata);

    response = trax_server_wait(_trax_handle, &_trax_image, &_trax_objects, NULL);

    assert(response == TRAX_INITIALIZE);

#if defined(VOT_RGBD)
        strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
        strcpy(_image.depth, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_DEPTH)));
#elif defined(VOT_RGBT)
        strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
        strcpy(_image.ir, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_IR)));
#elif defined(VOT_IR)
        strcpy(_image.ir, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_IR)));
#else
        strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
#endif

    trax_server_reply(_trax_handle, _trax_objects);

    memset(_objects, 0, sizeof(vot_region*) * VOT_MAX_OBJECTS);

    for (j = 0; j < trax_object_list_count(_trax_objects); j++) {
        trax_region* object = trax_object_list_get(_trax_objects, j);
        _objects[j] = _trax_to_region(object);
    }

    trax_object_list_release(&_trax_objects);
    trax_image_list_clear(_trax_image);
    trax_image_list_release(&_trax_image);

    #ifdef VOT_MULTI_OBJECT
        return _objects;
    #else
        return _objects[0];
    #endif
}

/**
 * Stores results to the result file and frees memory. This function should be
 * called at the end of the tracking program.
 */
void VOT_PREFIX(vot_quit)() {
    int i;

    if (_trax_handle) {
        trax_cleanup(&_trax_handle);

        for (i = 0; i < VOT_MAX_OBJECTS; i++) {
            if (_objects[i]) {
                vot_region_release(&(_objects[i]));
                _objects[i] = NULL;
            }
        }

        return;
    }

}

/**
 * Returns the file name of the current frame. This function does not advance
 * the current position.
 */
const vot_image* VOT_PREFIX(vot_frame)() {

    assert (_trax_handle);

    int response;
    trax_image_list* _trax_image = NULL;
    trax_object_list* _trax_objects = NULL;

    if (_vot_sequence_position == 0) {
        _vot_sequence_position++;
        return &_image;
    }

    response = trax_server_wait(_trax_handle, &_trax_image, &_trax_objects, NULL);

    assert(_trax_objects == NULL || trax_object_list_count(_trax_objects) == 0);

    trax_object_list_release(&_trax_objects);

    if (response != TRAX_FRAME) {
        vot_quit();
        return NULL;
    }

#if defined(VOT_RGBD)
    strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
    strcpy(_image.depth, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_DEPTH)));
#elif defined(VOT_RGBT)
    strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
    strcpy(_image.ir, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_IR)));
#elif defined(VOT_IR)
    strcpy(_image.ir, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_IR)));
#else
    strcpy(_image.color, trax_image_get_path(trax_image_list_get(_trax_image, TRAX_CHANNEL_COLOR)));
#endif
    trax_image_list_clear(_trax_image);
    trax_image_list_release(&_trax_image);

    return &_image;

}

/**
 * Used to report position of the object. This function also advances the
 * current position.
 */
#ifdef VOT_MULTI_OBJECT

void VOT_PREFIX(vot_report)(vot_objects objects) {

    int i;

    assert (_trax_handle);

    for (i = 0; i < VOT_MAX_OBJECTS; i++) {
        if (!objects[i]) {
            break;
        }
    }

    trax_object_list* _objects = trax_object_list_create(i);

    for (i = 0; i < trax_object_list_count(_objects); i++) {
        trax_region* _trax_region = _region_to_trax(objects[i]);
        trax_object_list_set(_objects, i, _trax_region);
        trax_region_release(&_trax_region);
    }

    trax_server_reply(_trax_handle, _objects);
    trax_object_list_release(&_objects);

}

#else

void VOT_PREFIX(vot_report)(vot_region* region) {

    assert (_trax_handle);

    trax_object_list* _objects = trax_object_list_create(1);
    trax_region* _trax_region = _region_to_trax(region);
    trax_object_list_set(_objects, 0, _trax_region);
    trax_region_release(&_trax_region);

    trax_server_reply(_trax_handle, _objects);
    trax_object_list_release(&_objects);

}


void VOT_PREFIX(vot_report2)(vot_region* region, float confidence) {

    assert (_trax_handle);

    trax_object_list* _objects = trax_object_list_create(1);
    trax_region* _trax_region = _region_to_trax(region);
    trax_object_list_set(_objects, 0, _trax_region);
    trax_properties_set_float(trax_object_list_properties(_objects, 0), "confidence", confidence);
    trax_region_release(&_trax_region);
    trax_server_reply(_trax_handle, _objects);
    trax_object_list_release(&_objects);

}

#endif

int VOT_PREFIX(vot_end)() {

    return _trax_handle == NULL;

}

#endif

