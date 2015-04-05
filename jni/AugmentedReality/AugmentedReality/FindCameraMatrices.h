

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "Common.h"

//#undef __SFM__DEBUG__

bool CheckCoherentRotation(Mat_<double>& R);
bool TestTriangulation(const vector<CloudPoint>& pcloud, const Matx34d& P, vector<uchar>& status, float threshold);

Mat GetFundamentalMat(	const vector<KeyPoint>& imgpts1,
							const vector<KeyPoint>& imgpts2,
							vector<KeyPoint>& imgpts1_good,
							vector<KeyPoint>& imgpts2_good,
							vector<DMatch>& matches
#ifdef __SFM__DEBUG__
							,const Mat& = Mat(), const Mat& = Mat()
#endif
						  );

bool FindCameraMatrices(const Mat& K, 
						const Mat& Kinv, 
						const Mat& distcoeff,
						const vector<KeyPoint>& imgpts1,
						const vector<KeyPoint>& imgpts2,
						vector<KeyPoint>& imgpts1_good,
						vector<KeyPoint>& imgpts2_good,
						Matx34d& P,
						Matx34d& P1,
						vector<DMatch>& matches,
						vector<CloudPoint>& outCloud
#ifdef __SFM__DEBUG__
						,const Mat& = Mat(), const Mat& = Mat()
#endif
						);
