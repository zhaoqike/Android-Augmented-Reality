

#pragma once

#include "MultiCameraDistance.h"
#include "Common.h"
#include "SfMUpdateListener.h"

class MultiCameraPnP : public MultiCameraDistance {
	vector<CloudPoint> pointcloud_beforeBA;
	vector<Vec3b> pointCloudRGB_beforeBA;

public:
	MultiCameraPnP(
		const vector<Mat>& imgs_, 
		const vector<string>& imgs_names_, 
		const string& imgs_path_):
	MultiCameraDistance(imgs_,imgs_names_,imgs_path_) 
	{
	}

	virtual void RecoverDepthFromImages();

	vector<Point3d> getPointCloudBeforeBA() { return CloudPointsToPoints(pointcloud_beforeBA); }
	const vector<Vec3b>& getPointCloudRGBBeforeBA() { return pointCloudRGB_beforeBA; }

private:
	void PruneMatchesBasedOnF();
	void AdjustCurrentBundle();
	void GetBaseLineTriangulation();
	void Find2D3DCorrespondences(int working_view, 
		vector<Point3f>& ppcloud, 
		vector<Point2f>& imgPoints);
	bool FindPoseEstimation(
		int working_view,
		Mat_<double>& rvec,
		Mat_<double>& t,
		Mat_<double>& R,
		vector<Point3f> ppcloud,
		vector<Point2f> imgPoints);
	bool TriangulatePointsBetweenViews(
		int working_view, 
		int second_view,
		vector<struct CloudPoint>& new_triangulated,
		vector<int>& add_to_cloud, 
		float threshold
		);
	
	int FindHomographyInliers2Views(int vi, int vj);
	int m_first_view;
	int m_second_view; //baseline's second view other to 0
	set<int> done_views;
	set<int> good_views;
	
/********** Subject / Objserver **********/
	vector < SfMUpdateListener * > listeners;
public:
    void attach(SfMUpdateListener *sul)
    {
        listeners.push_back(sul);
    }
private:
    void update()
    {
        for (int i = 0; i < listeners.size(); i++)
			listeners[i]->update(getPointCloud(),
								 getPointCloudRGB(),
								 getPointCloudBeforeBA(),
								 getPointCloudRGBBeforeBA(),
								 getCameras());
    }
};
