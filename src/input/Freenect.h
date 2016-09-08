/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of the class Freenect.
 *
 *   @author Lionel Heng <hengli@student.ethz.ch>
 *
 */

#ifndef FREENECT_H
#define FREENECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libfreenect/libfreenect.h>
#include <QMatrix4x4>
#include <QMutex>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QThread>
#include <QVector>
#include <QVector2D>
#include <QVector3D>


#define     FREENECT_FRAME_W   640
#define     FREENECT_FRAME_H   480
#define     FREENECT_FRAME_PIX   (FREENECT_FRAME_H*FREENECT_FRAME_W)
#define     FREENECT_IR_FRAME_W   640
#define     FREENECT_IR_FRAME_H   488
#define     FREENECT_IR_FRAME_PIX   (FREENECT_IR_FRAME_H*FREENECT_IR_FRAME_W)
#define     FREENECT_VIDEO_RGB_SIZE   (FREENECT_FRAME_PIX*3)
#define     FREENECT_VIDEO_BAYER_SIZE   (FREENECT_FRAME_PIX)
#define     FREENECT_VIDEO_YUV_RGB_SIZE   (FREENECT_VIDEO_RGB_SIZE)
#define     FREENECT_VIDEO_YUV_RAW_SIZE   (FREENECT_FRAME_PIX*2)
#define     FREENECT_VIDEO_IR_8BIT_SIZE   (FREENECT_IR_FRAME_PIX)
#define     FREENECT_VIDEO_IR_10BIT_SIZE   (FREENECT_IR_FRAME_PIX*sizeof(uint16_t))
#define     FREENECT_VIDEO_IR_10BIT_PACKED_SIZE   390400
#define     FREENECT_DEPTH_11BIT_SIZE   (FREENECT_FRAME_PIX*sizeof(uint16_t))
#define     FREENECT_DEPTH_10BIT_SIZE   FREENECT_DEPTH_11BIT_SIZE
#define     FREENECT_DEPTH_11BIT_PACKED_SIZE   422400
#define     FREENECT_DEPTH_10BIT_PACKED_SIZE   384000
#define     FREENECT_COUNTS_PER_G   819
#define     EXPORT


#define MAKE_RESERVED(res, fmt) (uint32_t)(((res & 0xff) << 8) | (((fmt & 0xff))))
#define RESERVED_TO_RESOLUTION(reserved) (freenect_resolution)((reserved >> 8) & 0xff)
#define RESERVED_TO_FORMAT(reserved) ((reserved) & 0xff)

#define video_mode_count 12
static freenect_frame_mode supported_video_modes[video_mode_count] = {
	// reserved, resolution, format, bytes, width, height, data_bits_per_pixel, padding_bits_per_pixel, framerate, is_valid
	{MAKE_RESERVED(FREENECT_RESOLUTION_HIGH,   FREENECT_VIDEO_RGB), FREENECT_RESOLUTION_HIGH, {FREENECT_VIDEO_RGB}, 1280*1024*3, 1280, 1024, 24, 0, 10, 1 },
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_RGB}, 640*480*3, 640,  480, 24, 0, 30, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_HIGH,   FREENECT_VIDEO_BAYER), FREENECT_RESOLUTION_HIGH, {FREENECT_VIDEO_BAYER}, 1280*1024, 1280, 1024, 8, 0, 10, 1 },
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_BAYER), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_BAYER}, 640*480, 640, 480, 8, 0, 30, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_HIGH,   FREENECT_VIDEO_IR_8BIT), FREENECT_RESOLUTION_HIGH, {FREENECT_VIDEO_IR_8BIT}, 1280*1024, 1280, 1024, 8, 0, 10, 1 },
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_IR_8BIT), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_IR_8BIT}, 640*488, 640, 488, 8, 0, 30, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_HIGH,   FREENECT_VIDEO_IR_10BIT), FREENECT_RESOLUTION_HIGH, {FREENECT_VIDEO_IR_10BIT}, 1280*1024*2, 1280, 1024, 10, 6, 10, 1 },
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_IR_10BIT), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_IR_10BIT}, 640*488*2, 640, 488, 10, 6, 30, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_HIGH,   FREENECT_VIDEO_IR_10BIT_PACKED), FREENECT_RESOLUTION_HIGH, {FREENECT_VIDEO_IR_10BIT_PACKED}, 1280*1024*10/8, 1280, 1024, 10, 0, 10, 1 },
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_IR_10BIT_PACKED), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_IR_10BIT_PACKED}, 640*488*10/8, 640, 488, 10, 0, 30, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_YUV_RGB), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_YUV_RGB}, 640*480*3, 640, 480, 24, 0, 15, 1 },

	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_YUV_RAW), FREENECT_RESOLUTION_MEDIUM, {FREENECT_VIDEO_YUV_RAW}, 640*480*2, 640, 480, 16, 0, 15, 1 },
};

#define depth_mode_count 6
static freenect_frame_mode supported_depth_modes[depth_mode_count] = {
	// reserved, resolution, format, bytes, width, height, data_bits_per_pixel, padding_bits_per_pixel, framerate, is_valid
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_11BIT}, 640*480*2, 640, 480, 11, 5, 30, 1},
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_10BIT), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_10BIT}, 640*480*2, 640, 480, 10, 6, 30, 1},
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT_PACKED), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_11BIT_PACKED}, 640*480*11/8, 640, 480, 11, 0, 30, 1},
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_10BIT_PACKED), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_10BIT_PACKED}, 640*480*10/8, 640, 480, 10, 0, 30, 1},
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_REGISTERED}, 640*480*2, 640, 480, 16, 0, 30, 1},
	{MAKE_RESERVED(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM), FREENECT_RESOLUTION_MEDIUM, {FREENECT_DEPTH_MM}, 640*480*2, 640, 480, 16, 0, 30, 1},
};
static const freenect_frame_mode invalid_mode = {0, (freenect_resolution)0, {(freenect_video_format)0}, 0, 0, 0, 0, 0, 0, 0};


//const freenect_frame_mode FREENECT_VIDEO_RGB = supported_video_modes[5];

class Freenect
{
public:
    Freenect();
    ~Freenect();

    bool init(int userDeviceNumber = 0);
    bool process(void);

    QSharedPointer<QByteArray> getRgbData(void);
    QSharedPointer<QByteArray> getRawDepthData(void);
    QSharedPointer<QByteArray> getColoredDepthData(void);
    QSharedPointer< QVector<QVector3D> > get3DPointCloudData(void);

    typedef struct {
        double x;
        double y;
        double z;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } Vector6D;
    QSharedPointer< QVector<Vector6D> > get6DPointCloudData();

    int getTiltAngle(void) const;
    void setTiltAngle(int angle);


private:
    typedef struct {
        // coordinates of principal point
        double cx;
        double cy;

        // focal length in pixels
        double fx;
        double fy;

        // distortion parameters
        double k[5];

    } IntrinsicCameraParameters;

    void readConfigFile(void);

    void rectifyPoint(const QVector2D& originalPoint,
                      QVector2D& rectifiedPoint,
                      const IntrinsicCameraParameters& params);
    void unrectifyPoint(const QVector2D& rectifiedPoint,
                        QVector2D& originalPoint,
                        const IntrinsicCameraParameters& params);
    void projectPixelTo3DRay(const QVector2D& pixel, QVector3D& ray,
                             const IntrinsicCameraParameters& params);

    static void videoCallback(freenect_device* device, void* video, uint32_t timestamp);
    static void depthCallback(freenect_device* device, void* depth, uint32_t timestamp);

    freenect_context* context;
    freenect_device* device;

    class FreenectThread : public QThread
    {
    public:
        explicit FreenectThread(freenect_device* _device);

    protected:
        virtual void run(void);

        freenect_device* device;
    };
    QScopedPointer<FreenectThread> thread;

    IntrinsicCameraParameters rgbCameraParameters;
    IntrinsicCameraParameters depthCameraParameters;

    QMatrix4x4 transformMatrix;
    double baseline;
    double disparityOffset;

    // tilt angle of Kinect camera
    int tiltAngle;

    // rgbd data
    char rgb[FREENECT_VIDEO_RGB_SIZE];
    QMutex rgbMutex;

    char depth[FREENECT_DEPTH_11BIT_SIZE];
    QMutex depthMutex;

    char coloredDepth[FREENECT_VIDEO_RGB_SIZE];
    QMutex coloredDepthMutex;

    // accelerometer data
    double ax, ay, az;
    double dx, dy, dz;

    // gamma map
    unsigned short gammaTable[2048];

    QVector3D depthProjectionMatrix[FREENECT_FRAME_PIX];
    QVector2D rgbRectificationMap[FREENECT_FRAME_PIX];

    // variables for use outside class
    QSharedPointer<QByteArray> rgbData;
    QSharedPointer<QByteArray> rawDepthData;
    QSharedPointer<QByteArray> coloredDepthData;
    QSharedPointer< QVector<QVector3D> > pointCloud3D;
    QSharedPointer< QVector<Vector6D> > pointCloud6D;
};

#endif // FREENECT_H
