

#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>

#include "IDistance.h"
#include "Triangulation.h"
#include "IFeatureMatcher.h"
#include "FindCameraMatrices.h"


class MultiCameraDistance  : public IDistance {	
protected:
	vector<vector<KeyPoint> > imgpts;
	vector<vector<KeyPoint> > fullpts;
	vector<vector<KeyPoint> > imgpts_good;

	map<pair<int,int> ,vector<DMatch> > matches_matrix;
	
	vector<Mat_<Vec3b> > imgs_orig;
	vector<Mat> imgs;
	vector<string> imgs_names;
	
	map<int,Matx34d> Pmats;

	Mat K;
	Mat_<double> Kinv;
	
	Mat cam_matrix,distortion_coeff;
	Mat distcoeff_32f; 
	Mat K_32f;

	vector<CloudPoint> pcloud;
	vector<Vec3b> pointCloudRGB;
	vector<KeyPoint> correspImg1Pt; //TODO: remove
	
	Ptr<IFeatureMatcher> feature_matcher;
	
	bool features_matched;
public:
	bool use_rich_features;
	bool use_gpu;

	vector<Point3d> getPointCloud() { return CloudPointsToPoints(pcloud); }
	const Mat& get_im_orig(int frame_num) { return imgs_orig[frame_num]; }
	const vector<KeyPoint>& getcorrespImg1Pt() { return correspImg1Pt; }
	const vector<Vec3b>& getPointCloudRGB() { if(pointCloudRGB.size()==0) { GetRGBForPointCloud(pcloud,pointCloudRGB); } return pointCloudRGB; }
	vector<Matx34d> getCameras() { 
		vector<Matx34d> v; 
		for(map<int ,Matx34d>::const_iterator it = Pmats.begin(); it != Pmats.end(); ++it ) {
			v.push_back( it->second );
		}
		return v;
    }

	void GetRGBForPointCloud(
		const vector<CloudPoint>& pcloud,
		vector<Vec3b>& RGBforCloud
		);

	MultiCameraDistance(
		const vector<Mat>& imgs_, 
		const vector<string>& imgs_names_, 
		const string& imgs_path_);	
	virtual void OnlyMatchFeatures(int strategy = STRATEGY_USE_FEATURE_MATCH);	
//	bool CheckCoherentRotation(Mat_<double>& R);
};
