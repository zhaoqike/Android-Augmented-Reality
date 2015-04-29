

#pragma once

#pragma warning(disable: 4244 18 4996 4800)
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <iostream>
#include <list>
#include <set>
using namespace std;
using namespace cv;
#include "State.h"
#include "CloudPoint.h"
#include "MyKeyPoint.h"

#define USE_SYS_TRIANGULATION 0

extern bool isPrint;

//global timer to compute frame rate
extern Timer gtimer;

//global last frame time
extern double lastglbtime;

//store frame rate
extern vector<double> frames;



const int defaultNum = 500;
const int defaultInitNum = 500;
const int defaultTrackNum = 500;
const int descriptorLen = 32;

const float initTriangulationThreshold = 0.75;
const float trackTriangulationThreshold = 0.75;

const float initProjectionErrorThreshold = 100.0;

struct MatHeader
{
	int rows;
	int cols;
	int type;
};

struct CloudPointHeader
{
	char sig[3];
	int indexInImageSize;
	MatHeader descriptorHeader;
};

//struct CloudPoint {
//	Point3d pt;
//	vector<int> imgpt_for_img;
//	double reprojection_error;
//	//Matx<uchar, 1, descriptorLen> descriptor;
//	//Mat_<int> descriptor = (Mat_<int>(32, 1));
//	Mat descriptor;
//};
//struct CPoint {
//	Point3d pt;
//	vector<int> imgpt_for_img;
//	double reprojection_error;
//	Matx34d p;
//	//Mat_<int> descriptor = (Mat_<int>(32, 1));
//};

struct DI
{
	int index;
	double distance;
};

bool CompareDI(DI a, DI b);




vector<DMatch> FlipMatches(const vector<DMatch>& matches);
void KeyPointsToPoints(const vector<KeyPoint>& kps, vector<Point2f>& ps);
void PointsToKeyPoints(const vector<Point2f>& ps, vector<KeyPoint>& kps);

vector<Point3d> CloudPointsToPoints(const vector<CloudPoint> cpts);

void GetAlignedPointsFromMatch(const vector<KeyPoint>& imgpts1,
							   const vector<KeyPoint>& imgpts2,
							   const vector<DMatch>& matches,
							   vector<KeyPoint>& pt_set1,
							   vector<KeyPoint>& pt_set2);

void drawArrows(Mat& frame, const vector<Point2f>& prevPts, const vector<Point2f>& nextPts, const vector<uchar>& status, const vector<float>& verror, const Scalar& line_color = Scalar(0, 0, 255));

#ifdef USE_PROFILING
#define CV_PROFILE(msg,code)	{\
	cout << msg << " ";\
	double __time_in_ticks = (double)getTickCount();\
	{ code }\
	cout << "DONE " << ((double)getTickCount() - __time_in_ticks)/getTickFrequency() << "s" << endl;\
}
#else
#define CV_PROFILE(msg,code) code
#endif

void open_imgs_dir(string dir_name, vector<Mat>& images, vector<string>& images_names, double downscale_factor);
void imshow_250x250(const string& name_, const Mat& patch);



const int maxKeyframeNum=200;

const string mapPath="mapstore";


string intToString(int num);




struct PC
{
	int i,j;
	int count;
	Matx34d P;
	Matx34d P1;
	Matx33d R;
	Matx33d R1;
	Matx33d Rinv;
	Matx33d R1inv;
};

void redirectStdout(string path);
void redirectStderr(string path);

void CloudPointsToPoints(vector<CloudPoint>& cloudPoints, vector<Point3f>& points3d);
