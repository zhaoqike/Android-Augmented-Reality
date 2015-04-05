#ifndef BUNDLENAJUSTER
#define BUNDLENAJUSTER

#include <vector>
#include <opencv2/core/core.hpp>
#include "Common.h"

class BundleAdjuster {
public:
	void adjustBundle(vector<CloudPoint>& pointcloud, 
					  Mat& cam_matrix,
					  const vector<vector<KeyPoint> >& imgpts,
					  map<int ,Matx34d>& Pmats);
	void adjustRecentBundle(CloudMap &cloudmap,int queryIndex,int trainIndex,vector<DMatch> &matches,Mat& cam_matrix);
private:
	int Count2DMeasurements(const vector<CloudPoint>& pointcloud);
	int CountRecent2DMeasurements(const vector<CloudPoint>& pointcloud);
};

#endif
