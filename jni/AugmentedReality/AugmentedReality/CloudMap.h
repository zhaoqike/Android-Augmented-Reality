#pragma once
#include "stdafx.h"
#include "MapPoint.h"
#include "KeyFrame.h"
#include "Render.h"


struct MapHeader
{
	char sig[3];
	int mappointSize;
	int keyframeSize;
};


class CloudMap
{
public:

	int threshold;
	int numLostFrames;
	int numAfterLastKeyFrame;
	double minKeyFrameDistance;

	

	string path;
	
	Mat K;
	Mat_<double> Kinv;
	
	Mat cam_matrix,distortion_coeff;
	Mat distcoeff_32f; 
	Mat K_32f;
	vector<KeyPoint> correspImg1Pt; //TODO: remove
	vector<CloudPoint> pcloud;
	vector<Vec3b> pointCloudRGB;
	vector<CloudPoint> pointcloud_beforeBA;
	vector<Vec3b> pointCloudRGB_beforeBA;
	vector<Matx34d> Pmats;
	//vector<Vec3b> pointCloudRGB;

	//vector<vector<KeyPoint> > imgpts;
	//vector<vector<KeyPoint> > fullpts;
	//vector<vector<KeyPoint> > imgpts_good;
	int XRange;
	int YRange;




	vector<MapPoint> mapPoints;
	vector<KeyFrame> keyFrames;
	//MultiCameraPnP distance;


	Mat_<double> estimatedT;
	Mat_<double> estimatedR;
	Mat_<double> estimatedRvec;

	//render
	bool isTriangulating;
	Render render;



	//vector to store error
	vector<double> projectError;


	///used for optical flow
	Mat lastImage;
	vector<Point3f> lastMapPoints;
	vector<Point2f> lastKeyPoints;
	vector<DMatch> lastMatches;




	CloudMap(void);
	~CloudMap(void);
	bool NeedFirst();
	bool NeedSecond();
	int InitMap();
	int MakeKeyFrame(Mat& image);
	int MakeKeyFrameLite(KeyFrame& keyframe,Mat& image,int num=defaultNum);
	int TrackFrame(Mat& image);
	int FirstBranch(KeyFrame& keyframe, Mat& image);
	int SecondBranch(KeyFrame& keyframe, Mat& image);
	int ThirdBranch(KeyFrame& keyframe, Mat& image);
	int OpticalBranch(Mat& image);
	int AddNewKeyFrame(KeyFrame& keyframe, int index);
	bool NeedNewKeyFrame(KeyFrame& keyframe, int& index);
	int ClosestKeyFrameDistance(KeyFrame& keyframe,double& outDistance);
	vector<int> kClosestKeyFrameDistance(KeyFrame& keyframe, int k);

	double ComputeDistance(KeyFrame& keyframe1,KeyFrame& keyframe2);

	int StoreMap(string filename);
	int StoreKeyFrame(fstream &file,KeyFrame &keyframe);
	int StoreKeyFrames(fstream &file);
	int StoreMapPoint(fstream &file,CloudPoint &cp);
	int StoreMapPoints(fstream &file);
	int StoreMapHeaderInfo(fstream &file);
	int StoreKeyFrameHeaderInfo(fstream &file,KeyFrame &keyframe);
	int StoreMapPointHeaderInfo(fstream &file,CloudPoint &cp);

	bool LoadMap(string filename);
	bool LoadKeyFrame(fstream &file,KeyFrame &keyframe);
	bool LoadKeyFrames(fstream &file,int keyframeSize);
	bool LoadMapPoint(fstream &file,CloudPoint &cp);
	bool LoadMapPoints(fstream &file,int mappointSize);
	bool LoadMapHeaderInfo(fstream &file,MapHeader &header);
	bool LoadKeyFrameHeaderInfo(fstream &file,KeyFrameHeader &keyframeHeader);
	bool LoadMapPointHeaderInfo(fstream &file,CloudPointHeader &cpHeader);

	int writeCloud();
	int writeCloud(string p);
	int writePmats();
	Point3d ComputeCenter();
	void writeMap();
	void fillPoseVal(float val[]);
	void writeError();
	//double getDistance(KeyFrame& k1,KeyFrame& k2);


	int SetCamera(Size size);
	



	void GetBaseLineTriangulation(vector<DMatch> &matches);

	bool TriangulatePointsBetweenViews(
		int query_view,
		int train_view,
		vector<DMatch> &matches,
		vector<struct CloudPoint>& new_triangulated,
		vector<int>& add_to_cloud,
		float threshold);

	bool TriangulatePointsBetweenKeyFrames(
		KeyFrame& query_view,
		KeyFrame& train_view,
		vector<DMatch> &matches,
		vector<struct CloudPoint>& new_triangulated,
		vector<int>& add_to_cloud);

	void AdjustCurrentBundle(int queryIdx,int trainIdx,vector<DMatch> &matches);

	void Find2D3DCorrespondencesWithNearestKeyFrame(int working_view, 
		KeyFrame& nowframe,
		vector<DMatch>& matches,
		vector<Point3f>& ppcloud, 
		vector<Point2f>& imgPoints);

	void Find2D3DCorrespondencesWithTriangulation(
		KeyFrame& nowframe,
		vector<DMatch>& matches,
		vector<Point3f>& ppcloud,
		vector<Point2f>& imgPoints);

	bool IsInImage(Point2f& point);

	void Find2D3DCorrespondencesWithNothing(
		KeyFrame& nowframe,
		vector<DMatch>& matches,
		vector<Point3f>& ppcloud,
		vector<Point2f>& imgPoints);

	bool FindPoseEstimation(
		Mat_<double>& rvec,
		Mat_<double>& t,
		Mat_<double>& R,
		vector<Point3f> &ppcloud,
		vector<Point2f> &imgPoints,
		vector<int> &inliers,
		Mat& image
		); 

	void GetRGBForPointCloud(
		const vector<struct CloudPoint>& _pcloud,
		vector<Vec3b>& RGBforCloud);

	vector<Vec3b>& getPointCloudRGB();

	void ExtractFeaturesFromMatches(KeyFrame& queryFrame, KeyFrame& trainFrame, vector<DMatch>& matches);
};

