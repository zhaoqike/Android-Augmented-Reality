/*
 * Common.h
 *
 *  Created on: 2014Äê8ÔÂ17ÈÕ
 *      Author: ZhaoQike
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/GraphicsWindow>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/Drawable>
#include <osg/DrawPixels>
#include <osg/MatrixTransform>
#include <iostream>

using namespace std;
using namespace cv;
extern bool captureFlag;
osg::ref_ptr<osg::Camera> createHUDBg(osg::ref_ptr<osg::Image> image);
osg::ref_ptr<osg::Camera> createHUDBg();
void readwrite();
osg::ref_ptr<osg::Image> matToOsgImage(Mat& mat);
Mat osgImageToMat(osg::ref_ptr<osg::Image> image);
void mergeImage(Mat& background,Mat& model);


#endif /* COMMON_H_ */

