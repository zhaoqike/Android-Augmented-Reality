/*
 * Map.cpp

 *
 *  Created on: 2014Äê6ÔÂ4ÈÕ
 *      Author: zhaoqike
 */
#include "Map.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <android/log.h>

#define LOG_TAG "FaceDetection/DetectionBasedTracker"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

//static int flag=0;
int r=10;
int Map::findfeature(long long addrGray, long long addrRgba)
{
	Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	vector<KeyPoint> fbkeypoints;

	FastFeatureDetector detector(100);
	BriefDescriptorExtractor brief;
	vector<KeyPoint> fbkeyPoints;
	Mat fbdescriptors;
	Timer t;
	t.start();
	detector.detect(mGr, fbkeypoints);
	int s=fbkeypoints.size();
	double d1=t.getElapsedTimeInMilliSec();
	LOGD("orb begin");



	int x,y;
	x=fbdescriptors.rows;
	y=fbdescriptors.cols;
	//orb(mRgb, Mat(), keyPoints, descriptors);

	brief.compute(mGr, fbkeypoints, fbdescriptors);
	int x1,y1;
	x1=fbdescriptors.rows;
	y1=fbdescriptors.cols;
	int size=fbkeypoints.size();
	double d2=t.getElapsedTimeInMilliSec();
	for( unsigned int i = 0; i < fbkeypoints.size(); i++ )
	{
		const KeyPoint& kp = fbkeypoints[i];
		circle(mRgb, Point(kp.pt.x, kp.pt.y), r, Scalar(255,0,0,255));
	}
	double d3=t.getElapsedTimeInMilliSec();

	ORB orb(200,1.2f,1);;
	vector<KeyPoint> orbkeyPoints;
	Mat orbdescriptors;
	orb(mRgb, Mat(), orbkeyPoints, orbdescriptors);
	double d4=t.getElapsedTimeInMilliSec();
	for( unsigned int i = 0; i < orbkeyPoints.size(); i++ )
	{
		const KeyPoint& kp = orbkeyPoints[i];
		circle(mRgb, Point(kp.pt.x, kp.pt.y), r, Scalar(255,0,0,255));
	}

	cout<<"new frame"<<endl;
	cout<<d1<<"   "<<d2<<"   "<<d3<<"  "<<d4<<" ms"<<endl;
	cout<<x<<" "<<y<<" "<<x1<<" "<<y1<<" "<<endl;
	cout<<size<<"  "<<s<<endl;
	cout<<orbkeyPoints.size()<<endl;
	cout<<endl;

	/*FILE* stream=fopen("/sdcard/test.txt","w");
	if(NULL==stream)
	{

	}
	else
	{
		char data[]={'h','e','l','l','9','\n'};
		size_t count=sizeof(data)/sizeof(data[0]);
		if(count!=fwrite(data,sizeof(char),count,stream))
		{
		}
	}
	fflush(stream);
	fclose(stream);*/


	return 0;
}

int Map::test()
{
	return 0;
}

void Map::changeColor()
{
	r=20;
}

