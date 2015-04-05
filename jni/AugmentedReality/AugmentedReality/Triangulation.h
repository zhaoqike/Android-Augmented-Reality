

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#ifdef __SFM__DEBUG__
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#include <vector>

#include "Common.h"

/**
 From "Triangulation", Hartley, R.I. and Sturm, P., Computer vision and image understanding, 1997
 */
Mat_<double> LinearLSTriangulation(Point3d u,		//homogenous image point (u,v,1)
								   Matx34d P,		//camera 1 matrix
								   Point3d u1,		//homogenous image point in 2nd camera
								   Matx34d P1		//camera 2 matrix
								   );

#define EPSILON 0.0001
/**
 From "Triangulation", Hartley, R.I. and Sturm, P., Computer vision and image understanding, 1997
 */
Mat_<double> IterativeLinearLSTriangulation(Point3d u,	//homogenous image point (u,v,1)
											Matx34d P,			//camera 1 matrix
											Point3d u1,			//homogenous image point in 2nd camera
											Matx34d P1			//camera 2 matrix
											);

double TriangulatePoints(const vector<KeyPoint>& pt_set1, 
					   const vector<KeyPoint>& pt_set2, 
					   const Mat& K,
					   const Mat& Kinv,
					   const Mat& distcoeff,
					   const Matx34d& P,
					   const Matx34d& P1,
					   vector<CloudPoint>& pointcloud,
					   vector<KeyPoint>& correspImg1Pt);
