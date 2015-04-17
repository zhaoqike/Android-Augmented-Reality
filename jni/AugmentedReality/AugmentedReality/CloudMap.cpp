#include "StdAfx.h"
#include "CloudMap.h"
#include "YamlStorage.h"





CloudMap::CloudMap(void)
{
	threshold=100;
	numLostFrames=0;
	numAfterLastKeyFrame=0;
	minKeyFrameDistance=-1;


	estimatedT = (Mat_<double>(1, 3));
	estimatedR = (Mat_<double>(3, 3));
	estimatedRvec = (Mat_<double>(1, 3));

	isTriangulating = true;

}


CloudMap::~CloudMap(void)
{
}

bool CloudMap::NeedFirst()
{
	return keyFrames.size()==0;
}
bool CloudMap::NeedSecond()
{
	return keyFrames.size()==1;
}

int CloudMap::InitMap()
{
	vector<DMatch> matches;
	FeatureMatcher matcher;
	matcher.Match(keyFrames[1],keyFrames[0],matches);
	cout<<"out  match"<<matches.size()<<endl;

	cerr << "begin init map" << endl;
	cerr<<"keyframe 0: "<<keyFrames[0].keyPoints.size()<<"  "<<keyFrames[0].keyPointsGood.size()<<endl;
	cerr<<"keyframe 1: "<<keyFrames[1].keyPoints.size()<<"  "<<keyFrames[1].keyPointsGood.size()<<endl;

	cerr << "begin base line:" << endl;
	GetBaseLineTriangulation(matches);
	cerr<<"after base line: "<<matches.size()<<endl;
	/*Mat out;
	drawMatches(keyFrames[1]->image, keyFrames[1]->keyPoints, keyFrames[0]->image, keyFrames[0]->keyPoints,
		matches, out, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("MatchORB", out);
	cerr<<"before wait"<<endl;
	waitKey(0);*/
	cerr<<"end wait"<<endl;

	//for (int i = 0; i < pcloud.size(); i++)
	//{
	//	cout << i << endl;
	//	cout << Mat(pcloud[i].descriptor) << endl;
	//}
	/*ExtractFeaturesFromMatches(keyFrames[1], keyFrames[0], matches);
	cout << "after extracting" << endl;
	cout << keyFrames[0].keyPoints.size() << endl;
	cout << keyFrames[0].descriptors.size() << endl;
	cout << keyFrames[1].keyPoints.size() << endl;
	cout << keyFrames[1].descriptors.size() << endl;*/
	return 0;
}
int CloudMap::MakeKeyFrame(Mat& image)
{
	//cerr<<"make keyframe begin"<<endl;
	KeyFrame keyframe;
	MakeKeyFrameLite(keyframe, image);
	//cerr<<"keyframe lite out"<<endl;
	/*image=image.clone();

	keyframe->indexInMap.resize(keyframe->keyPoints.size());
	for(int i=0;i<keyframe->indexInMap.size();i++)
	{
	keyframe->indexInMap[i]=-1;
	}*/
	//keyframe->makeKeyFrame(image);
	//cerr<<"begin push back"<<endl;
	keyFrames.push_back(keyframe);
	//cerr<<"keyframe make end"<<endl;
	return 0;
}
int CloudMap::MakeKeyFrameLite(KeyFrame& keyframe,Mat& image,int num)
{
	cout << "/////////////////////////////////////////" << endl;
	cout << "begin make keyframe lite:" << num << endl;
	keyframe.image=image.clone();
	FeatureExtractor extractor(num);
	extractor.ExtractFeatures(keyframe, image);
	keyframe.indexInMap.resize(keyframe.keyPoints.size());
	cout<<"keypoints size"<<endl;
	cout<<keyframe.keyPoints.size()<<endl;
	cout<<"descriptores size"<<endl;
	cout<<keyframe.descriptors.rows<<endl;
	if(keyframe.keyPoints.size()>200)
	{
		threshold-=5;
	}
	else
	{
		threshold+=10;
	}
	for(int i=0;i<keyframe.indexInMap.size();i++)
	{
		keyframe.indexInMap[i]=-1;
	}
	//cerr<<"keyframelite end"<<endl;
	return 0;
}

int CloudMap::TrackFrame(Mat& image)
{
	KeyFrame nowframe;
	Timer tracktimer;
	tracktimer.start();
	double makekfstart = tracktimer.getElapsedTimeInMilliSec();
	MakeKeyFrameLite(nowframe, image, defaultTrackNum);
	double makekfend = tracktimer.getElapsedTimeInMilliSec();
	double makekftime = makekfend - makekfstart;
	cout << "make keyframe time: " << makekftime << endl;
	//bool pose_estimated = false;
	int state;
	/*pose_estimated = FirstBranch(nowframe,image);
	if (pose_estimated == true)
	{
	cout << "success using first branch" << endl;
	cerr << "success using first branch" << endl;
	return trackSuccess;
	}*/
	/*pose_estimated = SecondBranch(nowframe,image);
	if (pose_estimated == true)
	{
	cout << "success using second branch" << endl;
	cerr << "success using second branch" << endl;
	return 0;
	}*/

	double thdstart = tracktimer.getElapsedTimeInMilliSec();
	state = ThirdBranch(nowframe, image);
	double thdend = tracktimer.getElapsedTimeInMilliSec();
	double thdtime = thdend - thdstart;
	cout << "third branch time cost: " << thdtime << endl;
	if (state >= trackSuccess)
	{
		cout << "success using third branch" << endl;
		cerr << "success using third branch" << endl;
		return state;
	}
	else
	{
		cout << "failed estimated" << endl;
		cerr << "failed estimated" << endl;
	}
	state = SecondBranch(nowframe, image);
	if (state >= trackSuccess)
	{
		return state;
	}
	state = FirstBranch(nowframe, image);
	return state;
	/*int trainIdx=keyFrames.size()-1;
	double minDistance=DBL_MAX;
	//int trainIdx=ClosestKeyFrameDistance(nowframe,minDistance);


	vector<DMatch> matches;
	//FeatureMatcher matcher;
	//matcher.MaskMatch(nowframe,keyFrames[trainIdx],matches);


	Matx34d P1 = keyFrames[trainIdx].Pmats;
	Mat_<double> t = (Mat_<double>(1,3) << P1(0,3), P1(1,3), P1(2,3));
	Mat_<double> R = (Mat_<double>(3,3) << P1(0,0), P1(0,1), P1(0,2), 
		P1(1,0), P1(1,1), P1(1,2), 
		P1(2,0), P1(2,1), P1(2,2));
	Mat_<double> rvec(1,3); Rodrigues(R, rvec);

	int _i=0;

	//while (done_views.size() != imgs.size())
	//{
	//	//find image with highest 2d-3d correspondance [Snavely07 4.2]
	//	unsigned int max_2d3d_view = -1, max_2d3d_count = 0;
	vector<Point3f> max_3d; vector<Point2f> max_2d; vector<int> inliers;
	//	for (unsigned int _i=0; _i < imgs.size(); _i++) {
	//		if(done_views.find(_i) != done_views.end()) continue; //already done with this view

	//vector<Point3f> tmp3d; vector<Point2f> tmp2d;
	//		cout << imgs_names[_i] << ": ";
	Find2D3DCorrespondencesWithNearestKeyFrame(trainIdx,nowframe,matches,max_3d,max_2d);
	//	if(tmp3d.size() > max_2d3d_count) {
	//		max_2d3d_count = tmp3d.size();
	//		max_2d3d_view = _i;
	//		max_3d = tmp3d; max_2d = tmp2d;
	//	}
	//}
	//int i = 0;// max_2d3d_view; //highest 2d3d matching view

	//cout << "-------------------------- " << imgs_names[i] << " --------------------------\n";
	//done_views.insert(i); // don't repeat it for now

	pose_estimated = FindPoseEstimation(rvec,t,R,max_3d,max_2d,inliers);
	if(!pose_estimated)
	{
		numLostFrames++;
		cout<<"lost a frame"<<endl;
		return -1;
	}
	estimatedR = R;
	estimatedT = t;
	estimatedRvec = rvec;

	numLostFrames=0;
	numAfterLastKeyFrame++;

	//store estimated pose	
	nowframe.Pmats = Matx34d	(R(0,0),R(0,1),R(0,2),t(0),
		R(1,0),R(1,1),R(1,2),t(1),
		R(2,0),R(2,1),R(2,2),t(2));
	cout<<"frame pose " <<Mat(nowframe.Pmats)<<endl;
	// start triangulating with previous GOOD views
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view) 
	//{
	//int view = *done_view;
	//if( view == i ) continue; //skip current...

	//cout << " -> " << imgs_names[view] << endl;
	if(true)//(0&&NeedNewKeyFrame(*nowframe))
	{
		vector<DMatch> inliersMatch;
		inliersMatch.clear();
		cout<<"next to triangulate ,inliers size: "<<inliers.size()<<endl;
		cout << "matches size: " << matches.size() << endl;
		for(int i=0;i<inliers.size();i++)
		{
			if (inliers[i]>matches.size())
			{
				cout << inliers[i] << endl;
			}
			inliersMatch.push_back(matches[inliers[i]]);
		}
		cout<<"before inliers match size: "<<matches.size()<<endl;
		matches=inliersMatch;
		cout<<"after inliers match size: "<<matches.size()<<endl;


		vector<CloudPoint> new_triangulated;
		vector<int> add_to_cloud;

		//add keyframe
		keyFrames.push_back(nowframe);
		numAfterLastKeyFrame=0;


		bool good_triangulation = TriangulatePointsBetweenViews(keyFrames.size()-1,trainIdx,matches,new_triangulated,add_to_cloud);
		if(!good_triangulation) 
		{
			//vector<KeyFrame>::iterator iter;
			keyFrames.erase(keyFrames.end() - 1);
			return -1;
		}

		cout << "before triangulation: " << pcloud.size();
		/*for (int j=0; j<add_to_cloud.size(); j++) {
		if(add_to_cloud[j] == 1)
		pcloud.push_back(new_triangulated[j]);
		}*/
		//cout << " after " << pcloud.size() << endl;
		//break;
		//}
		//good_views.insert(i);

		//AdjustCurrentBundle(keyFrames.size()-1,trainIdx,matches);
		//update();
	//}
}
//right
int CloudMap::FirstBranch(KeyFrame& nowframe,Mat& image)
{
	int trainIdx = 1;// keyFrames.size() - 1;
	cout << "train index is: " << trainIdx << endl;
	double minDistance = DBL_MAX;
	//int trainIdx=ClosestKeyFrameDistance(nowframe,minDistance);


	
	//FeatureMatcher matcher;
	//matcher.MaskMatch(nowframe,keyFrames[trainIdx],matches);


	/*Matx34d P1 = keyFrames[trainIdx].Pmats;
	Mat_<double> t = (Mat_<double>(1, 3) << P1(0, 3), P1(1, 3), P1(2, 3));
	Mat_<double> R = (Mat_<double>(3, 3) << P1(0, 0), P1(0, 1), P1(0, 2),
		P1(1, 0), P1(1, 1), P1(1, 2),
		P1(2, 0), P1(2, 1), P1(2, 2));
	Mat_<double> rvec(1, 3); Rodrigues(R, rvec);

	int _i = 0;*/

	//while (done_views.size() != imgs.size())
	//{
	//	//find image with highest 2d-3d correspondance [Snavely07 4.2]
	//	unsigned int max_2d3d_view = -1, max_2d3d_count = 0;
	vector<Point3f> points3d; 
	vector<Point2f> points2d; 
	vector<int> inliers;
	vector<DMatch> matches;
	//	for (unsigned int _i=0; _i < imgs.size(); _i++) {
	//		if(done_views.find(_i) != done_views.end()) continue; //already done with this view

	//vector<Point3f> tmp3d; vector<Point2f> tmp2d;
	//		cout << imgs_names[_i] << ": ";
	Find2D3DCorrespondencesWithNearestKeyFrame(trainIdx, nowframe, matches, points3d, points2d);
	//	if(tmp3d.size() > max_2d3d_count) {
	//		max_2d3d_count = tmp3d.size();
	//		max_2d3d_view = _i;
	//		max_3d = tmp3d; max_2d = tmp2d;
	//	}
	//}
	//int i = 0;// max_2d3d_view; //highest 2d3d matching view

	//cout << "-------------------------- " << imgs_names[i] << " --------------------------\n";
	//done_views.insert(i); // don't repeat it for now


	/*Matx34d P1 = keyFrames[trainIdx].Pmats;
	Mat_<double> t = (Mat_<double>(1, 3) << P1(0, 3), P1(1, 3), P1(2, 3));
	Mat_<double> R = (Mat_<double>(3, 3) << P1(0, 0), P1(0, 1), P1(0, 2),
		P1(1, 0), P1(1, 1), P1(1, 2),
		P1(2, 0), P1(2, 1), P1(2, 2));
	Mat_<double> rvec(1, 3); Rodrigues(R, rvec);*/
	bool pose_estimated = FindPoseEstimation(estimatedRvec, estimatedT, estimatedR, points3d, points2d, inliers,image);

	if (!pose_estimated)
	{
		numLostFrames++;
		cout << "lost a frame" << endl;
		return trackFailed;
	}


	//draw match points in image
	/*for (int i = 0; i < inliers.size(); i++)
	{
		int index = inliers[i];
		Point2f& p = points2d[index];
		circle(image, Point(p.x, p.y), 5, Scalar(1.0, 0.0, 0.0));
	}*/

	//imshow("image", image);
	//waitKey(0);


	numLostFrames = 0;
	numAfterLastKeyFrame++;
	Mat_<double> R(estimatedR);
	Mat_<double> t(estimatedT);
	//store estimated pose	
	nowframe.Pmats = Matx34d(R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));
	cout << "frame pose " << Mat(nowframe.Pmats) << endl;
	//push into pmats
	Pmats.push_back(nowframe.Pmats);
	// start triangulating with previous GOOD views
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view) 
	//{
	//int view = *done_view;
	//if( view == i ) continue; //skip current...

	//cout << " -> " << imgs_names[view] << endl;
	int index;
	double distance;
	if (NeedNewKeyFrame(nowframe,index))
	{
		vector<DMatch> inliersMatch;
		inliersMatch.clear();
		cout << "next to triangulate ,inliers size: " << inliers.size() << endl;
		cout << "matches size: " << matches.size() << endl;
		for (int i = 0; i<inliers.size(); i++)
		{
			if (inliers[i]>matches.size())
			{
				cout << inliers[i] << endl;
			}
			inliersMatch.push_back(matches[inliers[i]]);
		}
		cout << "before inliers match size: " << matches.size() << endl;
		matches = inliersMatch;
		cout << "after inliers match size: " << matches.size() << endl;


		vector<CloudPoint> new_triangulated;
		vector<int> add_to_cloud;

		//add keyframe
		keyFrames.push_back(nowframe);
		


		bool good_triangulation = TriangulatePointsBetweenViews(keyFrames.size() - 1, trainIdx, matches, new_triangulated, add_to_cloud,trackTriangulationThreshold);
		if (!good_triangulation)
		{
			//vector<KeyFrame>::iterator iter;
			keyFrames.erase(keyFrames.end() - 1);
			return triangulateFailed;
		}

		//triangulation success
		numAfterLastKeyFrame = 0;
		cout << "before triangulation: " << pcloud.size();
		/*for (int j=0; j<add_to_cloud.size(); j++) {
		if(add_to_cloud[j] == 1)
		pcloud.push_back(new_triangulated[j]);
		}*/
		cout << " after " << pcloud.size() << endl;
		//break;
		//}
		//good_views.insert(i);

		//AdjustCurrentBundle(keyFrames.size()-1,trainIdx,matches);
		//update();
	}


	return triangulateSuccess;
}
//middle
int CloudMap::SecondBranch(KeyFrame& nowframe,Mat& image)
{
	cout << "begin second branch" << endl;
	vector<Point3f> points3d; 
	vector<Point2f> points2d; 
	vector<int> inliers;
	vector<DMatch> matches;
	int state = 0;
	Timer secondtime;
	secondtime.start();
	double findcorresstart = secondtime.getElapsedTimeInMilliSec();
	Find2D3DCorrespondencesWithTriangulation(nowframe, matches, points3d, points2d);
	double findcorresend = secondtime.getElapsedTimeInMilliSec();
	double findcorrestime = findcorresend - findcorresstart;
	cout << "find corres cost: " << findcorrestime << endl;

	double poseestistart = secondtime.getElapsedTimeInMilliSec();
	bool pose_estimated = FindPoseEstimation(estimatedRvec, estimatedT, estimatedR, points3d, points2d, inliers,image);
	double poseestiend = secondtime.getElapsedTimeInMilliSec();
	double poseestitime = poseestiend - poseestistart;
	cout << "pose esti cost: " << poseestitime << endl;
	if (pose_estimated)
	{
		state = trackSuccess;
	}
	else
	{
		state = trackFailed;
		numLostFrames++;
		cout << "lost a frame" << endl;
		return state;
	}

	Mat_<double> R(estimatedR);
	Mat_<double> t(estimatedT);
	//store estimated pose	
	nowframe.Pmats = Matx34d(R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));
	cout << "frame pose " << Mat(nowframe.Pmats) << endl;
	//set number
	numLostFrames = 0;
	numAfterLastKeyFrame++;
	//push into pmats
	Pmats.push_back(nowframe.Pmats);
	int index;
	double distance;
	if (NeedNewKeyFrame(nowframe, index))
	{
		for (int i = 0; i < matches.size(); i++)
		{
			nowframe.indexInMap[matches[i].queryIdx] = matches[i].trainIdx;
		}
		vector<int> vdi = kClosestKeyFrameDistance(nowframe, 4);
		int triangulatestate = triangulateFailed;
		for (int i = 0; i < vdi.size(); i++)
		{
			triangulatestate = AddNewKeyFrame(nowframe, vdi[i]);
			if (triangulatestate == triangulateSuccess)
			{
				state=triangulateSuccess;
			}
		}
	}
	if (state == triangulateSuccess)
	{
		cout << "state is triangulate success" << endl;
		numAfterLastKeyFrame = 0;
	}
	return state;
}
//left
int CloudMap::ThirdBranch(KeyFrame& nowframe, Mat& image)
{
	cout << "begin third branch" << endl;
	vector<Point3f> points3d;
	vector<Point2f> points2d;
	vector<int> inliers;
	vector<DMatch> matches;
	int state = 0;
	Timer thirdtime;
	thirdtime.start();
	double findcorresstart=thirdtime.getElapsedTimeInMilliSec();
	Find2D3DCorrespondencesWithNothing(nowframe, matches, points3d, points2d);
	double findcorresend = thirdtime.getElapsedTimeInMilliSec();
	double findcorrestime = findcorresend - findcorresstart;
	cout << "find corres cost: " << findcorrestime << endl;
	
	double poseestistart = thirdtime.getElapsedTimeInMilliSec();
	bool pose_estimated = FindPoseEstimation(estimatedRvec, estimatedT, estimatedR, points3d, points2d, inliers,image);
	double poseestiend = thirdtime.getElapsedTimeInMilliSec();
	double poseestitime = poseestiend - poseestistart;
	cout << "pose esti cost: " << poseestitime << endl;
	if (pose_estimated)
	{
		state = trackSuccess;
	}
	else
	{
		state = trackFailed;
		numLostFrames++;
		cout << "lost a frame" << endl;
		return state;
	}


	Mat_<double> R(estimatedR);
	Mat_<double> t(estimatedT);
	//store estimated pose	
	nowframe.Pmats = Matx34d(R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));
	cout << "frame pose " << Mat(nowframe.Pmats) << endl;
	//set number
	numLostFrames = 0;
	numAfterLastKeyFrame++;
	//push into pmats
	Pmats.push_back(nowframe.Pmats);
	int index;
	double distance;
	if (NeedNewKeyFrame(nowframe, index))
	{
		for (int i = 0; i < matches.size(); i++)
		{
			nowframe.indexInMap[matches[i].queryIdx] = matches[i].trainIdx;
		}
		vector<int> vdi = kClosestKeyFrameDistance(nowframe, 4);
		int triangulatestate = triangulateFailed;
		for (int i = 0; i < vdi.size(); i++)
		{
			triangulatestate = AddNewKeyFrame(nowframe, vdi[i]);
			if (triangulatestate == triangulateSuccess)
			{
				state = triangulateSuccess;
				break;
			}
		}
	}
	if (state == triangulateSuccess)
	{
		cout << "state is triangulate success" << endl;
		numAfterLastKeyFrame = 0;
	}

	//for optical flow
	lastImage = nowframe.image;
	for (int i = 0; i < matches.size(); i++)
	{
		lastMapPoints.push_back(pcloud[matches[i].trainIdx].pt);
		lastKeyPoints.push_back(nowframe.keyPoints[matches[i].queryIdx].pt);
	}



	return state;
}

int CloudMap::OpticalBranch(Mat& image)
{
	int state = 0;
	vector<Point2f> nowPoints;
	vector<uchar> status;
	vector<float> errs;
	vector<int> inliers;


	calcOpticalFlowPyrLK(lastImage, image, lastKeyPoints, nowPoints, status, errs);


	//filter and swap(store)
	int k = 0;
	for (int i = 0; i < status.size(); i++)
	{
		if (status[i])
		{
			nowPoints[k] = nowPoints[i];
			lastMapPoints[k] = lastMapPoints[i];
			k++;
		}
	}
	nowPoints.resize(k);
	lastMapPoints.resize(k);
	swap(lastKeyPoints, nowPoints);
	swap(lastImage, image);


	bool pose_estimated = FindPoseEstimation(estimatedRvec, estimatedT, estimatedR, lastMapPoints, lastKeyPoints, inliers, image);


	if (pose_estimated)
	{
		state = trackSuccess;
	}
	else
	{
		state = trackFailed;
		numLostFrames++;
		cout << "lost a frame" << endl;
		return state;
	}


	Mat_<double> R(estimatedR);
	Mat_<double> t(estimatedT);
	//store estimated pose	
	Matx34d Pmat = Matx34d(R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));
	cout << "frame pose " << Mat(Pmat) << endl;
	//set number
	numLostFrames = 0;
	numAfterLastKeyFrame++;
	//push into pmats
	Pmats.push_back(Pmat);

	return state;


}

int CloudMap::AddNewKeyFrame(KeyFrame& keyframe,int index)
{
	keyFrames.push_back(keyframe);

	int queryIdx = keyFrames.size() - 1;
	int trainIdx = index;

	vector<CloudPoint> new_triangulated;
	vector<int> add_to_cloud;

	vector<DMatch> matches;
	FeatureMatcher matcher;
	matcher.Match(keyFrames[queryIdx], keyFrames[trainIdx], matches);
	

	cerr << "triangulate begins" << endl;
	bool good_triangulation = TriangulatePointsBetweenViews(queryIdx, trainIdx, matches, new_triangulated, add_to_cloud,trackTriangulationThreshold);
	cerr << "triangulate end" << endl;
	if (!good_triangulation) {
		cout << "triangulation failed" << endl;
		keyFrames.erase(keyFrames.end() - 1);
		return triangulateFailed;
		//Pmats[m_first_view] = 0;
		//Pmats[m_second_view] = 0;
		//m_second_view++;
	}
	else {
		//cout << "before triangulation: " << pcloud.size();
		//for (unsigned int j=0; j<add_to_cloud.size(); j++) {
		//	if(add_to_cloud[j] == 1)
		//		pcloud.push_back(new_triangulated[j]);
		//}
		cout << " after " << pcloud.size() << endl;
		return triangulateSuccess;
	}


}

bool CloudMap::NeedNewKeyFrame(KeyFrame& keyframe, int& index)
{
	//return false;
	if (numAfterLastKeyFrame<6 || keyFrames.size()>maxKeyframeNum)
	{
		return false;
	}

	int i;
	double d;


	


	i = ClosestKeyFrameDistance(keyframe, d);


	cout << "in need new keyframe" << endl;
	cout << i << "  " << d << "  " << minKeyFrameDistance << endl;



	if (d > minKeyFrameDistance)
	{
		return true;
	}
	return false;
}

int CloudMap::ClosestKeyFrameDistance(KeyFrame& keyframe,double& outDistance)
{
	double minDistance=DBL_MAX;
	int closest=keyFrames.size()-1;
	for(int i=0;i<keyFrames.size();i++)
	{
		double distance=ComputeDistance(keyframe,keyFrames[i]);
		cout<<"to "<<i<<"th keyframe distance is: "<<distance<<endl;
		if(distance<minDistance)
		{
			minDistance=distance;
			closest=i;
		}
	}
	outDistance=minDistance;
	cout<<"closest keyframe is: "<<closest<<" distance is: "<<minDistance<<endl;
	return closest;
}

vector<int> CloudMap::kClosestKeyFrameDistance(KeyFrame& keyframe, int k)
{
	vector<DI> vdi;
	for (int i = 0; i < keyFrames.size(); i++)
	{
		double d = ComputeDistance(keyframe, keyFrames[i]);
		DI di;
		di.index = i;
		di.distance = d;
		vdi.push_back(di);
	}
	sort(vdi.begin(), vdi.end(), CompareDI);


	//cout information
	cout << "in k closest keyframe distance"<<endl;
	cout << vdi.size() << endl;
	for (int i = 0; i<vdi.size(); i++)
	{
		cout << vdi[i].distance << "  " << vdi[i].index << endl;
	}


	int len = vdi.size()>k ? k : vdi.size();
	vector<int> result;
	for (int i = 0; i < len; i++)
	{
		result.push_back(vdi[i].index);
	}
	return result;
}

double CloudMap::ComputeDistance(KeyFrame& keyframe1,KeyFrame& keyframe2)
{
	Matx34d p1=keyframe1.Pmats;
	Matx34d p2=keyframe2.Pmats;

	Point3d t1(p1(0,3),p1(1,3),p1(2,3));
	Point3d t2(p2(0,3),p2(1,3),p2(2,3));
	double distanceSquare=(t1.x-t2.x)*(t1.x-t2.x)+(t1.y-t2.y)*(t1.y-t2.y)+(t1.z-t2.z)*(t1.z-t2.z);
	double distance=sqrt(distanceSquare);
	return distance;
}


int CloudMap::StoreMap(string filename)
{
	FileStorage fs(filename,FileStorage::WRITE);
	fs << "mappoints" << pcloud;
	fs << "keyframes" << keyFrames;
	fs.release();


	/*fstream file;
	file.open(filename.c_str(),ios::out|ios::binary);
	StoreMapHeaderInfo(file);

	StoreMapPoints(file);
	StoreKeyFrames(file);*/
	return 0;
}

int CloudMap::StoreKeyFrame(fstream &file,KeyFrame &keyframe)
{
	StoreKeyFrameHeaderInfo(file,keyframe);
	//keypoints
	for(int i=0;i<keyframe.keyPoints.size();i++)
	{
		file.write((char*)(&keyframe.keyPoints[i]),sizeof(keyframe.keyPoints[i]));
		
	}
	//index in map
	for(int i=0;i<keyframe.indexInMap.size();i++)
	{
		file.write((char*)(&keyframe.indexInMap[i]),sizeof(keyframe.indexInMap[i])); 
	}
	//descriptors
	file.write((char*)(keyframe.descriptors.data),sizeof(keyframe.descriptors.total()*keyframe.descriptors.elemSize()));
	//pmats
	file.write((char*)(keyframe.Pmats.val),sizeof(keyframe.Pmats.val));
	return 0;
}
int CloudMap::StoreKeyFrames(fstream &file)
{
	for(int i=0;i<keyFrames.size();i++)
	{
		//KeyFrame keyframe=*(keyFrames[i]);
		StoreKeyFrame(file,keyFrames[i]);
	}
	return 0;
}
int CloudMap::StoreMapPoint(fstream &file,CloudPoint &cp)
{
	StoreMapPointHeaderInfo(file,cp);
	cout << "after header: " << file.tellg() << endl;

	file.write((char*)(&cp.pt),sizeof(cp.pt));
	file.write((char*)(&cp.reprojection_error),sizeof(cp.reprojection_error));

	//descriptor size
	file.write((char*)(&cp.descriptor.rows), sizeof(cp.descriptor.rows));
	file.write((char*)(&cp.descriptor.cols), sizeof(cp.descriptor.cols));
	cout << "after size: " << file.tellg() << endl;


	for(int i=0;i<cp.imgpt_for_img.size();i++)
	{
		file.write((char*)(&cp.imgpt_for_img[i]),sizeof(cp.imgpt_for_img[i]));
	}
	cout << "after index: " << file.tellg() << endl;

	//descriptors
	file.write((char*)(cp.descriptor.data), cp.descriptor.total()*cp.descriptor.elemSize());
	cout << "after des: " << file.tellg() << endl;


	return 0;
}
int CloudMap::StoreMapPoints(fstream &file)
{
	for(int i=0;i<pcloud.size();i++)
	{
		cout << "position:  " << i << "  " << file.tellg() << "  " << file.tellp() << endl;
		StoreMapPoint(file,pcloud[i]);
		
	}
	return 0;
}

int CloudMap::StoreMapHeaderInfo(fstream &file)
{
	MapHeader header;
	header.sig[0]='m';
	header.sig[1]='h';
	header.sig[2]='d';
	header.mappointSize=pcloud.size();
	header.keyframeSize=keyFrames.size();
	file.write((char*)(&header),sizeof(header));
	return 0;
}

int CloudMap::StoreKeyFrameHeaderInfo(fstream &file,KeyFrame &keyframe)
{
	KeyFrameHeader kfh;
	kfh.sig[0]='k';
	kfh.sig[1]='f';
	kfh.sig[2]='m';
	kfh.keyPointsSize=keyframe.keyPoints.size();
	kfh.IndexInMapSize=keyframe.indexInMap.size();

	kfh.descriptorHeader.rows=keyframe.descriptors.rows;
	kfh.descriptorHeader.cols=keyframe.descriptors.cols;
	kfh.descriptorHeader.type=keyframe.descriptors.type();

	//kfh.pheader.rows=keyframe.Pmats.rows;
	//kfh.pheader.cols=keyframe.Pmats.cols;
	//kfh.pheader.type=keyframe.Pmats.type();

	file.write((char*)(&kfh),sizeof(kfh));
	return 0;
}
int CloudMap::StoreMapPointHeaderInfo(fstream &file,CloudPoint &cp)
{
	CloudPointHeader cph;
	cph.sig[0]='m';
	cph.sig[1]='p';
	cph.sig[2]='t';
	cph.indexInImageSize=cp.imgpt_for_img.size();

	cph.descriptorHeader.rows = cp.descriptor.rows;
	cph.descriptorHeader.cols = cp.descriptor.cols;
	cph.descriptorHeader.type = cp.descriptor.type();



	file.write((char*)(&cph),sizeof(cph));
	return 0;
}



bool CloudMap::LoadMap(string filename)
{
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
	{
		cout << "load open failed" << endl;
	}
	fs["mappoints"] >> pcloud;
	fs["keyframes"] >> keyFrames;
	fs.release();



	/*fstream file;
	file.open(filename.c_str(),ios::in|ios::binary);
	MapHeader header;
	bool flag=true;
	flag=LoadMapHeaderInfo(file,header);
	cout<<"&&"<<header.sig[0]<<"&&"<<header.sig[1]<<"&&"<<header.sig[2]<<endl;
	if(false==flag)
	{
		cout<<"load header fail"<<endl;
		return false;
	}
	cout<<"load header success"<<endl;
	cout<<header.keyframeSize<<endl;
	cout<<header.mappointSize<<endl;
	flag=LoadMapPoints(file,header.mappointSize);
	if(false==flag)
	{
		cout<<"load map points fail"<<endl;
		return false;
	}
	cout<<"load map points success"<<endl;
	flag=LoadKeyFrames(file,header.keyframeSize);
	if(false==flag)
	{
		cout<<"load keyframes fail"<<endl;
		return false;
	}
	cout<<"load keyframes success"<<endl;*/
	return 0;
}

bool CloudMap::LoadKeyFrame(fstream &file,KeyFrame &keyframe)
{
	KeyFrameHeader header;
	bool flag=true;
	flag=LoadKeyFrameHeaderInfo(file,header);
	if(false==flag)
	{
		return false;
	}
	keyframe.keyPoints.resize(header.keyPointsSize);
	keyframe.indexInMap.resize(header.IndexInMapSize);
	keyframe.descriptors.create(header.descriptorHeader.rows,header.descriptorHeader.cols,header.descriptorHeader.type);

	//keypoints
	for(int i=0;i<keyframe.keyPoints.size();i++)
	{
		file.read((char*)(&keyframe.keyPoints[i]),sizeof(keyframe.keyPoints[i]));
	}
	//index in map
	for(int i=0;i<keyframe.indexInMap.size();i++)
	{
		file.read((char*)(&keyframe.indexInMap[i]),sizeof(keyframe.indexInMap[i])); 
	}
	//descriptors
	file.read((char*)(keyframe.descriptors.data),sizeof(keyframe.descriptors.total()*keyframe.descriptors.elemSize()));
	//pmats
	file.read((char*)(keyframe.Pmats.val),sizeof(keyframe.Pmats.val));
	return true;
}
bool CloudMap::LoadKeyFrames(fstream &file,int keyframeSize)
{
	keyFrames.resize(keyframeSize);
	bool flag=true;
	for(int i=0;i<keyframeSize;i++)
	{
		flag=LoadKeyFrame(file,keyFrames[i]);
		if(false==flag)
		{
			return false;
		}
	}
	return true;
}
bool CloudMap::LoadMapPoint(fstream &file,CloudPoint &cp)
{
	CloudPointHeader header;
	bool flag=true;
	flag=LoadMapPointHeaderInfo(file,header);



	
	if(false==flag)
	{
		cout << "load map point header failed" << endl;
		return false;
	}
	cp.imgpt_for_img.resize(header.indexInImageSize);
	cp.descriptor.create(header.descriptorHeader.rows, header.descriptorHeader.cols, header.descriptorHeader.type);
	file.read((char*)(&cp.pt),sizeof(cp.pt));
	file.read((char*)(&cp.reprojection_error),sizeof(cp.reprojection_error));



	for(int i=0;i<cp.imgpt_for_img.size();i++)
	{
		file.read((char*)(&cp.imgpt_for_img[i]),sizeof(cp.imgpt_for_img[i]));
	}
	
	//descriptors
	file.read((char*)(cp.descriptor.data), cp.descriptor.total()*cp.descriptor.elemSize());


	return true;
}

bool CloudMap::LoadMapPoints(fstream &file,int mappointSize)
{
	pcloud.resize(mappointSize);
	bool flag=true;
	for(int i=0;i<mappointSize;i++)
	{
		flag=LoadMapPoint(file,pcloud[i]);
		if(false==flag)
		{
			cout<<"mappoint "<<i<<" failed"<<endl;
			return false;
		}
	}
	return true;
}

bool CloudMap::LoadMapHeaderInfo(fstream &file,MapHeader &header)
{
	file.read((char*)(&header),sizeof(header));
	if ('m' == header.sig[0] && 'h' == header.sig[1] && 'd' == header.sig[2]){
		return true;
	}
	return false;
}

bool CloudMap::LoadKeyFrameHeaderInfo(fstream &file,KeyFrameHeader &keyframeHeader)
{
	file.read((char*)(&keyframeHeader),sizeof(keyframeHeader));
	if('k'==keyframeHeader.sig[0] &&	'f'==keyframeHeader.sig[1] &&	'm'==keyframeHeader.sig[2])
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CloudMap::LoadMapPointHeaderInfo(fstream &file,CloudPointHeader &cpHeader)
{
	file.read((char*)(&cpHeader),sizeof(cpHeader));
	//cout<<"&&"<<cpHeader.sig[0]<<"&&"<<cpHeader.sig[1]<<"&&"<<cpHeader.sig[2]<<"&&"<<endl;
	if('m'==cpHeader.sig[0]&&	'p'==cpHeader.sig[1]&&	't'==cpHeader.sig[2])
	{
		return true;
	}
	else
	{
		cout << "map point header sig error: "<<cpHeader.sig[0] << cpHeader.sig[1] << cpHeader.sig[1] << endl;
		return false;
	}
}

int CloudMap::writeCloud()
{
	if (pointCloudRGB.size() < pcloud.size())
	{
		getPointCloudRGB();
	}
	fstream file;
	string out = path + "/out";
	file.open(out + "/cloud.txt", ios::out);
	if (!file.is_open())
	{
		cerr << "cloud file open failed" << endl;
	}
	file << pcloud.size() << endl;
	for (int i = 0; i < pcloud.size(); i++)
	{
		file << pcloud[i].pt.x << "  " << pcloud[i].pt.y << "  " << pcloud[i].pt.z << "  " << (int)pointCloudRGB[i].val[0] << "  " << (int)pointCloudRGB[i].val[1] << "  " << (int)pointCloudRGB[i].val[2] << "  " << endl;
	}
	file.close();
	//write pmats
	cerr << "in write cloud begin write pmats" << endl;
	fstream pmatfile;
	pmatfile.open(out + "/Pmats.txt", ios::out);
	if (!pmatfile.is_open())
	{
		cerr << "pmatfile open failed" << endl;
	}
	pmatfile << Pmats.size() << endl;
	cerr << Pmats.size() << endl;
	for (int i = 0; i < Pmats.size(); i++)
	{
		for (int j = 0; j < 12; j++)
		{
			pmatfile << Pmats[i].val[j] << "  ";
		}
		pmatfile << endl;
	}
	pmatfile.close();
	ComputeCenter();

	/*fstream imagenamefile;
	imagenamefile.open(out + "imagename.txt", ios::out);
	if (!imagenamefile.is_open())
	{
	cerr << "imagename file open failed" << endl;
	}
	imagenamefile<<*/
	return 0;
}

int CloudMap::writeCloud(string p)
{
	string tmp = path;
	path = p;
	writeCloud();
	path = tmp;
	return 0;
}

int CloudMap::writePmats()
{
	for (int i = 0; i < keyFrames.size(); i++)
	{
		imwrite(path + "/outimage/" + intToString(i) + ".jpg", keyFrames[i].image);
	}
	FileStorage fsout(path+"/Pmats.xml", FileStorage::WRITE);
	for (int i = 0; i < keyFrames.size(); i++)
	{
		cerr << i << endl;
		Mat p(keyFrames[i].Pmats);
		//cerr << p << endl;
		string s = intToString(i);
		s = "Pmats" + s;
		cout << s << endl;
		fsout << s << p;
	}
	fsout.release();
	return 0;
}

//double CloudMap::getDistance(KeyFrame& k1,KeyFrame& k2)
//{
//	Point3d p1=k1.Pmats.at






int CloudMap::SetCamera(Size size)
{
	FileStorage fs;
	if(fs.open("out_camera_data.yml",FileStorage::READ)) {
		fs["camera_matrix"]>>cam_matrix;
		fs["distortion_coefficients"]>>distortion_coeff;
	} else {
		//no calibration matrix file - mockup calibration
		double max_w_h = MAX(size.height,size.width);
		cam_matrix = (Mat_<double>(3,3) <<	max_w_h ,	0	,		size.width/2.0,
			0,			max_w_h,	size.height/2.0,
			0,			0,			1);
		distortion_coeff = Mat_<double>::zeros(1,4);
	}


	K = cam_matrix;
	invert(K, Kinv); //get inverse of camera matrix

	distortion_coeff.convertTo(distcoeff_32f,CV_32FC1);
	K.convertTo(K_32f,CV_32FC1);

	//set xrange yrange
	XRange = size.width;
	YRange = size.height;
	return 0;
}








void CloudMap::GetBaseLineTriangulation(vector<DMatch> &matches) {
	cout << "=========================== Baseline triangulation ===========================\n";
	cerr<<"get baseline triangulate begin"<<endl;
	Matx34d P_train(1,0,0,0,
		0,1,0,0,
		0,0,1,0),
		P_query(1,0,0,0,
		0,1,0,0,
		0,0,1,0);

	vector<CloudPoint> tmp_pcloud;

	//sort pairwise matches to find the lowest Homography inliers [Snavely07 4.2]
	//cout << "Find highest match...";
	//list<pair<int,pair<int,int> > > matches_sizes;
	////TODO: parallelize!
	//for(map<pair<int,int> ,vector<DMatch> >::iterator i = matches_matrix.begin(); i != matches_matrix.end(); ++i) {
	//	if((*i).second.size() < 100)
	//		matches_sizes.push_back(make_pair(100,(*i).first));
	//	else {
	//		int Hinliers = FindHomographyInliers2Views((*i).first.first,(*i).first.second);
	//		int percent = (int)(((double)Hinliers) / ((double)(*i).second.size()) * 100.0);
	//		cout << "[" << (*i).first.first << "," << (*i).first.second << " = "<<percent<<"] ";
	//		matches_sizes.push_back(make_pair((int)percent,(*i).first));
	//	}
	//}
	//cout << endl;
	//matches_sizes.sort(sort_by_first);

	//Reconstruct from two views
	bool goodF = false;
	int highest_pair = 0;
	int trainIdx =0;
	int queryIdx = 1;




	//reverse iterate by number of matches
	//for(list<pair<int,pair<int,int> > >::iterator highest_pair = matches_sizes.begin(); 
	//	highest_pair != matches_sizes.end() && !goodF; 
	//	++highest_pair) 
	//{
	//	m_second_view = (*highest_pair).second.second;
	//	m_first_view  = (*highest_pair).second.first;

	//cout << " -------- " << imgs_names[m_first_view] << " and " << imgs_names[m_second_view] << " -------- " <<endl;
	//what if reconstrcution of first two views is bad? fallback to another pair
	//See if the Fundamental Matrix between these two views is good
	goodF = FindCameraMatrices(K, Kinv, distortion_coeff,
		keyFrames[queryIdx].keyPoints, 
		keyFrames[trainIdx].keyPoints, 
		keyFrames[queryIdx].keyPointsGood,
		keyFrames[trainIdx].keyPointsGood, 
		P_query, 
		P_train,
		matches,
		tmp_pcloud
#ifdef __SFM__DEBUG__
		,imgs[m_first_view],imgs[m_second_view]
#endif
	);
	if (goodF) {
		vector<CloudPoint> new_triangulated;
		vector<int> add_to_cloud;

		keyFrames[trainIdx].Pmats = P_train;
		keyFrames[queryIdx].Pmats = P_query;
		
		//push pmats into vector
		Pmats.push_back(P_train);
		Pmats.push_back(P_query);

		//set now estimated position and rotation
		Mat_<double> t = (Mat_<double>(1, 3) << P_query(0, 3), P_query(1, 3), P_query(2, 3));
		Mat_<double> R = (Mat_<double>(3, 3) << P_query(0, 0), P_query(0, 1), P_query(0, 2),
												P_query(1, 0), P_query(1, 1), P_query(1, 2),
												P_query(2, 0), P_query(2, 1), P_query(2, 2));
		Mat_<double> rvec(1, 3); Rodrigues(R, rvec);

		estimatedR = R;
		estimatedT = t;
		estimatedRvec = rvec;






		cerr<<"triangulate begins"<<endl;
		bool good_triangulation = TriangulatePointsBetweenViews(queryIdx,trainIdx,matches,new_triangulated,add_to_cloud,initTriangulationThreshold);
		cerr<<"triangulate end"<<endl;
		if(!good_triangulation || countNonZero(add_to_cloud) < 10) {
			cout << "triangulation failed" << endl;
			goodF = false;
			//Pmats[m_first_view] = 0;
			//Pmats[m_second_view] = 0;
			//m_second_view++;
		} else {
			//cout << "before triangulation: " << pcloud.size();
			//for (unsigned int j=0; j<add_to_cloud.size(); j++) {
			//	if(add_to_cloud[j] == 1)
			//		pcloud.push_back(new_triangulated[j]);
			//}
			cout << " after " << pcloud.size() << endl;
			minKeyFrameDistance = ComputeDistance(keyFrames[queryIdx], keyFrames[trainIdx]);
			cout << "min keyframe distance" << endl;
			cout << minKeyFrameDistance << endl;
		}				
	}
	//}

	/*if (!goodF) {
	cerr << "Cannot find a good pair of images to obtain a baseline triangulation" << endl;
	exit(0);
	}*/

	//cout << "Taking baseline from " << imgs_names[m_first_view] << " and " << imgs_names[m_second_view] << endl;

	//	double reproj_error;
	//	{
	//		vector<KeyPoint> pt_set1,pt_set2;
	//		
	//		vector<DMatch> matches = matches_matrix[make_pair(m_first_view,m_second_view)];
	//
	//		GetAlignedPointsFromMatch(imgpts[m_first_view],imgpts[m_second_view],matches,pt_set1,pt_set2);
	//		
	//		pcloud.clear();
	//		reproj_error = TriangulatePoints(pt_set1, 
	//										 pt_set2, 
	//										 Kinv, 
	//										 distortion_coeff,
	//										 Pmats[m_first_view], 
	//										 Pmats[m_second_view], 
	//										 pcloud, 
	//										 correspImg1Pt);
	//		
	//		for (unsigned int i=0; i<pcloud.size(); i++) {
	//			pcloud[i].imgpt_for_img = vector<int>(imgs.size(),-1);
	//			//matches[i] corresponds to pointcloud[i]
	//			pcloud[i].imgpt_for_img[m_first_view] = matches[i].queryIdx;
	//			pcloud[i].imgpt_for_img[m_second_view] = matches[i].trainIdx;
	//		}
	//	}
	//	cout << "triangulation reproj error " << reproj_error << endl;
}


bool CloudMap::TriangulatePointsBetweenViews(
	int query_view, 
	int train_view,
	vector<DMatch> &matches,
	vector<CloudPoint>& new_triangulated,
	vector<int>& add_to_cloud,
	float threshold
	) 
{
	//cout << " Triangulate " << imgs_names[working_view] << " and " << imgs_names[older_view] << endl;
	//get the left camera matrix
	//TODO: potential bug - the P mat for <view> may not exist? or does it...
	Matx34d P_query = keyFrames[query_view].Pmats;
	Matx34d P_train = keyFrames[train_view].Pmats;

	cout<<"begin triangulation"<<endl;
	cout<<"p query index "<<query_view<<endl;
	cout<<Mat(P_query)<<endl;
	cout<<"p train index "<<train_view<<endl;
	cout<<Mat(P_train)<<endl;

	vector<KeyPoint> pt_queryset,pt_trainset;
	cout<<"getalign"<<endl;
	GetAlignedPointsFromMatch(keyFrames[query_view].keyPoints,keyFrames[train_view].keyPoints,matches,pt_queryset,pt_trainset);


	//adding more triangulated points to general cloud
	cout<<"triangulate points"<<endl;
	cout << matches.size() << "  " << pt_queryset.size() << "  " << pt_trainset.size() << endl;
	double reproj_error = TriangulatePoints(pt_queryset, pt_trainset, K, Kinv, distortion_coeff, P_query, P_train, new_triangulated, correspImg1Pt);
	cout << "triangulation reproj error " << reproj_error << endl;

	vector<uchar> trig_status;
	cout<<"test triangulate"<<endl;
	if(!TestTriangulation(new_triangulated, P_query, trig_status,threshold) || !TestTriangulation(new_triangulated, P_train, trig_status,threshold)) {
		cout << "Triangulation did not succeed" << endl;
		return false;
	}
	//	if(reproj_error > 20.0) {
	//		// somethign went awry, delete those triangulated points
	//		//				pcloud.resize(start_i);
	//		cerr << "reprojection error too high, don't include these points."<<endl;
	//		return false;
	//	}

	//filter out outlier points with high reprojection
	vector<double> reprj_errors;
	for(int i=0;i<new_triangulated.size();i++) { reprj_errors.push_back(new_triangulated[i].reprojection_error); }
	sort(reprj_errors.begin(),reprj_errors.end());
	//get the 80% precentile
	double reprj_err_cutoff = reprj_errors[4 * reprj_errors.size() / 5] * 2.4; //threshold from Snavely07 4.2


	reprj_err_cutoff = 10.0;
	cout << "reprj error threshold: " << reprj_err_cutoff << endl;

	vector<CloudPoint> new_triangulated_filtered;
	vector<DMatch> new_matches;
	for(int i=0;i<new_triangulated.size();i++) {
		if(trig_status[i] == 0)
			continue; //point was not in front of camera
		if(new_triangulated[i].reprojection_error > 16.0) {
			continue; //reject point
		} 
		if(new_triangulated[i].reprojection_error < 4.0 ||
			new_triangulated[i].reprojection_error < reprj_err_cutoff) 
		{
			new_triangulated_filtered.push_back(new_triangulated[i]);
			new_matches.push_back(matches[i]);
		} 
		else 
		{
			continue;
		}
	}

	cout << "filtered out " << (new_triangulated.size() - new_triangulated_filtered.size()) << " high-error points" << endl;




	//add in triangulation function
	// use for test cut later
	cout << "add in triangulation function" << endl;
	cout << matches.size() << "  " << pt_queryset.size() << "  " << pt_trainset.size() << endl;
	GetAlignedPointsFromMatch(keyFrames[query_view].keyPoints, keyFrames[train_view].keyPoints, new_matches, pt_queryset, pt_trainset);
	vector<CloudPoint> mynew_triangulated;
	double myreproj_error = TriangulatePoints(pt_queryset, pt_trainset, K, Kinv, distortion_coeff, P_query, P_train, mynew_triangulated, correspImg1Pt);
	cout << "my reproj error: " << myreproj_error << endl;




	//all points filtered?
	if(new_triangulated_filtered.size() <= 0) return false;

	new_triangulated.swap(new_triangulated_filtered);

	matches.swap(new_matches);
	//matches_matrix[make_pair(older_view,working_view)] = new_matches; //just to make sure, remove if unneccesary
	//matches_matrix[make_pair(working_view,older_view)] = FlipMatches(new_matches);
	add_to_cloud.clear();
	add_to_cloud.resize(new_triangulated.size(),1);
	int found_other_views_count = 0;
	int num_views = keyFrames.size();

	//scan new triangulated points, if they were already triangulated before - strengthen cloud
	//#pragma omp parallel for num_threads(1)
	cout<<"my work begins"<<endl;
	for (int j = 0; j<new_triangulated.size(); j++) {
		//cerr<<j<<endl;
		new_triangulated[j].imgpt_for_img = vector<int>(maxKeyframeNum,-1);

		//matches[j] corresponds to new_triangulated[j]
		//matches[j].queryIdx = point in <older_view>
		//matches[j].trainIdx = point in <working_view>
		new_triangulated[j].imgpt_for_img[query_view] = matches[j].queryIdx;	//2D reference to <older_view>
		new_triangulated[j].imgpt_for_img[train_view] = matches[j].trainIdx;		//2D reference to <working_view>
		Mat dst = Mat(new_triangulated[j].descriptor);
		Mat src = keyFrames[train_view].descriptors.row(matches[j].trainIdx);
		src.copyTo(new_triangulated[j].descriptor);


		//cout << "src" << endl;
		//cout << Mat(src) << endl;
		//cout << "dst" << endl;
		//cout << Mat(dst) << endl;
		//cout << "descriptors" << endl;
		//cout << new_triangulated[j].descriptor << endl;

		bool found_in_other_view = false;
		int trainIdx=matches[j].trainIdx;
		int queryIdx=matches[j].queryIdx;
		/*if(j==295)
		{
		cerr<<"older"<<endl;
		cerr<<query_view<<"  "<<train_view<<endl;
		cerr<<matches.size()<<endl;
		cerr<<matches[j].queryIdx<<"  "<<matches[j].trainIdx<<endl;
		}
		if(j==295)
		{
		cerr<<keyFrames[keyFrames.size()-1]->indexInMap.size()<<endl;
		}*/
		int pt3d = -1;
		int pt3dfromtrain = keyFrames[train_view].indexInMap[trainIdx];
		int pt3dfromquery = keyFrames[query_view].indexInMap[queryIdx];
		if (pt3dfromtrain != -1)
		{
			pt3d = pt3dfromtrain;
		}
		else if (pt3dfromquery != -1)
		{
			pt3d = pt3dfromquery;
		}
		/*if(j==295)
		{
		cerr<<keyFrames[keyFrames.size()-1]->indexInMap.size()<<endl;
		}*/
		if(pt3d!=-1)
		{
			found_in_other_view=true;
			pcloud[pt3d].imgpt_for_img[train_view] = matches[j].trainIdx;
			pcloud[pt3d].imgpt_for_img[query_view] = matches[j].queryIdx;
			keyFrames[train_view].indexInMap[trainIdx]=pt3d;
			keyFrames[query_view].indexInMap[queryIdx]=pt3d;
			found_in_other_view = true;
			add_to_cloud[j] = 0;
		}
		else
		{

			pcloud.push_back(new_triangulated[j]);
			pt3d=pcloud.size()-1;
			pcloud.back().imgpt_for_img[train_view] = matches[j].trainIdx;
			pcloud.back().imgpt_for_img[query_view] = matches[j].queryIdx;
			keyFrames[train_view].indexInMap[trainIdx]=pt3d;
			keyFrames[query_view].indexInMap[queryIdx]=pt3d;
		}

		{
			if (found_in_other_view) {
				found_other_views_count++;
			} else {
				add_to_cloud[j] = 1;
			}
		}
	}
	cout << found_other_views_count << "/" << new_triangulated.size() << " points were found in other views, adding " << countNonZero(add_to_cloud) << " new\n";
	return true;
}

bool CloudMap::TriangulatePointsBetweenKeyFrames(
	KeyFrame& query_view,
	KeyFrame& train_view,
	vector<DMatch> &matches,
	vector<CloudPoint>& new_triangulated,
	vector<int>& add_to_cloud)
{
	return false;
}

void CloudMap::AdjustCurrentBundle(int queryIdx,int trainIdx,vector<DMatch> &matches) {
	cout << "======================== Bundle Adjustment ==========================\n";

	/*pointcloud_beforeBA = pcloud;
	//GetRGBForPointCloud(pointcloud_beforeBA,pointCloudRGB_beforeBA);

	Mat _cam_matrix = K;
	BundleAdjuster BA;
	map<int,Matx34d> Pmats;
	BA.adjustRecentBundle(*this,queryIdx,trainIdx,matches,_cam_matrix);
	K = cam_matrix;
	Kinv = K.inv();

	cout << "use new K " << endl << K << endl;*/

	//GetRGBForPointCloud(pcloud,pointCloudRGB);
}	


void CloudMap::Find2D3DCorrespondencesWithNearestKeyFrame(int kftrainIdx,
	KeyFrame& nowframe,
	vector<DMatch>& matches,
	vector<Point3f>& ppcloud, 
	vector<Point2f>& imgPoints) 
{
	ppcloud.clear(); 
	imgPoints.clear();


	KeyFrame& keyframetrain=keyFrames[kftrainIdx];
	//vector<DMatch> matches;
	FeatureMatcher matcher;
	//vector<DMatch> matches;
	matcher.Match(nowframe,keyframetrain,matches);
	cout<<"-----------------------------------------------------------------"<<endl;
	cout<<"find 2d 3d correspondences with nearest keyframe"<<endl;
	cout<<"query frame keypoints: "<<nowframe.keyPoints.size()<<endl;
	cout<<"train frame keypoints: "<<keyframetrain.keyPoints.size()<<endl;
	cout<<"match size: "<<matches.size()<<endl;
	cout<<"-----------------------------------------------------------------"<<endl;

	for(int i=0;i<matches.size();i++)
	{
		int trainIdx=matches[i].trainIdx;
		int idxInMap=keyframetrain.indexInMap[trainIdx];
		if(idxInMap!=-1)
		{
			ppcloud.push_back(pcloud[idxInMap].pt);
			int queryIdx=matches[i].queryIdx;
			imgPoints.push_back(nowframe.keyPoints[queryIdx].pt);
		}
	}
	//vector<int> pcloud_status(pcloud.size(),0);
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view) 
	//{
	//	int old_view = *done_view;
	//	//check for matches_from_old_to_working between i'th frame and <old_view>'th frame (and thus the current cloud)
	//	vector<DMatch> matches_from_old_to_working = matches_matrix[make_pair(old_view,working_view)];

	//	for (unsigned int match_from_old_view=0; match_from_old_view < matches_from_old_to_working.size(); match_from_old_view++) {
	//		// the index of the matching point in <old_view>
	//		int idx_in_old_view = matches_from_old_to_working[match_from_old_view].queryIdx;

	//		//scan the existing cloud (pcloud) to see if this point from <old_view> exists
	//		for (unsigned int pcldp=0; pcldp<pcloud.size(); pcldp++) {
	//			// see if corresponding point was found in this point
	//			if (idx_in_old_view == pcloud[pcldp].imgpt_for_img[old_view] && pcloud_status[pcldp] == 0) //prevent duplicates
	//			{
	//				//3d point in cloud
	//				ppcloud.push_back(pcloud[pcldp].pt);
	//				//2d point in image i
	//				imgPoints.push_back(imgpts[working_view][matches_from_old_to_working[match_from_old_view].trainIdx].pt);

	//				pcloud_status[pcldp] = 1;
	//				break;
	//			}
	//		}
	//	}
	//}
	cout << "found " << ppcloud.size() << " 3d-2d point correspondences"<<endl;
}

void CloudMap::Find2D3DCorrespondencesWithTriangulation(KeyFrame& nowframe,
	vector<DMatch>& matches,
	vector<Point3f>& ppcloud,
	vector<Point2f>& imgPoints)
{
	cout << "begin find 2d 3d corresponddences with triangulation" << endl;
	ppcloud.clear();
	imgPoints.clear();

	vector<Point3f> allPoints;
	CloudPointsToPoints(pcloud, allPoints);
	cout << "after cloudpoints to points" << pcloud.size() << "  " << allPoints.size() << endl;
	vector<Point2f> projected3D;
	projectPoints(allPoints, estimatedRvec, estimatedT, K, distortion_coeff, projected3D);
	//cout << "after projecting points" << endl;
	//for (int i = 0; i < pcloud.size(); i++)
	//{
	//	cout << pcloud[i].pt.x << "  " << pcloud[i].pt.y << "  " << pcloud[i].pt.z << "  " << allPoints[i].x << "  " << allPoints[i].y <<"  "<<allPoints[i].z<< "  " << projected3D[i].x << "  " << projected3D[i].y << endl;
	//}
	vector<int> projected3DInImage;

	cout << "begin selecting points..." << endl;

	for (int i = 0; i < projected3D.size(); i++)
	{
		if (IsInImage(projected3D[i]))
		{
			projected3DInImage.push_back(i);
		}
	}


	Mat  cloudDescriptors(projected3DInImage.size(), descriptorLen, CV_8UC1);
	for (int i = 0; i < projected3DInImage.size(); i++)
	{
		Mat dst = cloudDescriptors.row(i);
		Mat src = Mat(pcloud[projected3DInImage[i]].descriptor);
		src.copyTo(dst);
	}
	FeatureMatcher matcher;
	//vector<DMatch> matches;

	matcher.MaskMatch(nowframe.descriptors, cloudDescriptors, matches);


	//KeyFrame& keyframetrain = keyFrames[kftrainIdx];
	//vector<DMatch> matches;
	//FeatureMatcher matcher;
	//matcher.Match(nowframe, keyframetrain, matches);
	cout << "-----------------------------------------------------------------" << endl;
	cout << "find 2d 3d correspondences with triangulation" << endl;
	cout << "query frame keypoints: " << nowframe.keyPoints.size() << endl;
	//cout << "train frame keypoints: " << keyframetrain.keyPoints.size() << endl;
	cout << "match size: " << matches.size() << endl;
	cout << "-----------------------------------------------------------------" << endl;

	for (int i = 0; i<matches.size(); i++)
	{
		int trainIdx = matches[i].trainIdx;
		int queryIdx = matches[i].queryIdx;
		if (queryIdx != -1)
		{
			ppcloud.push_back(pcloud[projected3DInImage[trainIdx]].pt);//projected3DInImage represent the index in original pcloud
			//int queryIdx = matches[i].queryIdx;
			imgPoints.push_back(nowframe.keyPoints[queryIdx].pt);
		}
	}
	//vector<int> pcloud_status(pcloud.size(),0);
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view) 
	//{
	//	int old_view = *done_view;
	//	//check for matches_from_old_to_working between i'th frame and <old_view>'th frame (and thus the current cloud)
	//	vector<DMatch> matches_from_old_to_working = matches_matrix[make_pair(old_view,working_view)];

	//	for (unsigned int match_from_old_view=0; match_from_old_view < matches_from_old_to_working.size(); match_from_old_view++) {
	//		// the index of the matching point in <old_view>
	//		int idx_in_old_view = matches_from_old_to_working[match_from_old_view].queryIdx;

	//		//scan the existing cloud (pcloud) to see if this point from <old_view> exists
	//		for (unsigned int pcldp=0; pcldp<pcloud.size(); pcldp++) {
	//			// see if corresponding point was found in this point
	//			if (idx_in_old_view == pcloud[pcldp].imgpt_for_img[old_view] && pcloud_status[pcldp] == 0) //prevent duplicates
	//			{
	//				//3d point in cloud
	//				ppcloud.push_back(pcloud[pcldp].pt);
	//				//2d point in image i
	//				imgPoints.push_back(imgpts[working_view][matches_from_old_to_working[match_from_old_view].trainIdx].pt);

	//				pcloud_status[pcldp] = 1;
	//				break;
	//			}
	//		}
	//	}
	//}
	cout << "found " << ppcloud.size() << " 3d-2d point correspondences" << endl;
}

bool CloudMap::IsInImage(Point2f& point)
{
	//cout << point.x << "  " << point.y << "  " << XRange << "  " << YRange << endl;
	if (point.x >= 0 && point.x <= XRange&&point.y >= 0 || point.y <= YRange)
	{
		return true;
	}
	return false;
}

void CloudMap::Find2D3DCorrespondencesWithNothing(
	KeyFrame& nowframe,
	vector<DMatch>& matches,
	vector<Point3f>& ppcloud,
	vector<Point2f>& imgPoints)
{
	cout << "begin find 2d 3d correspondences with nothing" << endl;
	Timer findtime;
	findtime.start();
	ppcloud.clear();
	imgPoints.clear();

	//vector<KeyPoint> cloudKeyPoints(pcloud.size());
	Mat  cloudDescriptors(pcloud.size(), descriptorLen, CV_8UC1);
	//cout << "before copy" << endl;
	//cout << cloudDescriptors << endl;
	double copystart=findtime.getElapsedTimeInMilliSec();
	for (int i = 0; i < pcloud.size(); i++)
	{
		Mat dst = cloudDescriptors.row(i);
		Mat src = Mat(pcloud[i].descriptor);
		src.copyTo(dst);
	}

	//cout << "after copy" << endl;
	//cout << cloudDescriptors << endl;
	FeatureMatcher matcher;
	//vector<DMatch> matches;
	cout << "nowframe descriptor size" << endl;
	cout << nowframe.descriptors.size() << endl;
	cout << "cloud descriptor size" << endl;
	cout << cloudDescriptors.size() << endl;
	cout << "matches size" << endl;
	cout << matches.size() << endl;

	matcher.MaskMatch(nowframe.descriptors, cloudDescriptors, matches);

	/*cout << "before affine" << endl;
	cout << matches.size() << endl;

	vector<Point3f> cloudPoints(matches.size());
	vector<Point3f> framePoints(matches.size());
	for (int i = 0; i < matches.size(); i++)
	{
		int trainIdx = matches[i].trainIdx;
		int queryIdx = matches[i].queryIdx;
		cloudPoints[i] = pcloud[trainIdx].pt;

		framePoints[i].x = nowframe.keyPoints[queryIdx].pt.x;
		framePoints[i].y = nowframe.keyPoints[queryIdx].pt.y;
		framePoints[i].z = 0.0f;
	}
	vector<int> inliers;
	Mat out;
	estimateAffine3D(cloudPoints, framePoints, out, inliers);
	vector<DMatch> new_matches(inliers.size());
	for (int i = 0; i < inliers.size(); i++)
	{
		new_matches[i] = matches[inliers[i]];
	}
	matches.swap(new_matches);

	cout << "after affine" << endl;
	cout << matches.size() << endl;*/




	//KeyFrame& keyframetrain = keyFrames[kftrainIdx];
	//vector<DMatch> matches;
	//FeatureMatcher matcher;
	//matcher.Match(nowframe, keyframetrain, matches);
	cout << "-----------------------------------------------------------------" << endl;
	cout << "find 2d 3d correspondences with nothing" << endl;
	cout << "query frame keypoints: " << nowframe.keyPoints.size() << endl;
	//cout << "train frame keypoints: " << keyframetrain.keyPoints.size() << endl;
	cout << "match size: " << matches.size() << endl;
	cout << "-----------------------------------------------------------------" << endl;

	for (int i = 0; i<matches.size(); i++)
	{
		int trainIdx = matches[i].trainIdx;
		int queryIdx = matches[i].queryIdx;
		if (queryIdx != -1)
		{
			nowframe.indexInMap[queryIdx] = trainIdx;
			ppcloud.push_back(pcloud[trainIdx].pt);
			//int queryIdx = matches[i].queryIdx;
			imgPoints.push_back(nowframe.keyPoints[queryIdx].pt);
		}
	}
	//vector<int> pcloud_status(pcloud.size(),0);
	//for (set<int>::iterator done_view = good_views.begin(); done_view != good_views.end(); ++done_view) 
	//{
	//	int old_view = *done_view;
	//	//check for matches_from_old_to_working between i'th frame and <old_view>'th frame (and thus the current cloud)
	//	vector<DMatch> matches_from_old_to_working = matches_matrix[make_pair(old_view,working_view)];

	//	for (unsigned int match_from_old_view=0; match_from_old_view < matches_from_old_to_working.size(); match_from_old_view++) {
	//		// the index of the matching point in <old_view>
	//		int idx_in_old_view = matches_from_old_to_working[match_from_old_view].queryIdx;

	//		//scan the existing cloud (pcloud) to see if this point from <old_view> exists
	//		for (unsigned int pcldp=0; pcldp<pcloud.size(); pcldp++) {
	//			// see if corresponding point was found in this point
	//			if (idx_in_old_view == pcloud[pcldp].imgpt_for_img[old_view] && pcloud_status[pcldp] == 0) //prevent duplicates
	//			{
	//				//3d point in cloud
	//				ppcloud.push_back(pcloud[pcldp].pt);
	//				//2d point in image i
	//				imgPoints.push_back(imgpts[working_view][matches_from_old_to_working[match_from_old_view].trainIdx].pt);

	//				pcloud_status[pcldp] = 1;
	//				break;
	//			}
	//		}
	//	}
	//}
	cout << "found " << ppcloud.size() << " 3d-2d point correspondences" << endl;
}


bool CloudMap::FindPoseEstimation(
	Mat_<double>& rvec,
	Mat_<double>& t,
	Mat_<double>& R,
	vector<Point3f> &ppcloud,
	vector<Point2f> &imgPoints,
	vector<int> &inliers,
	Mat& image
	) 
{
	if(ppcloud.size() <= 7 || imgPoints.size() <= 7 || ppcloud.size() != imgPoints.size()) { 
		//something went wrong aligning 3D to 2D points..
		cout << "couldn't find [enough] corresponding cloud points... (only " << ppcloud.size() << ")" <<endl;
		return false;
	}

	//vector<int> inliers;

	double minVal,maxVal; minMaxIdx(imgPoints,&minVal,&maxVal);
	Timer time;
	time.start();
	cout << "maxVal"<<maxVal << "  " << 0.006*maxVal << endl;
	double start=time.getElapsedTimeInMilliSec();
	double reprojectionError = 0.006 * maxVal;
	reprojectionError = 10;
	//solvePnPRansac(ppcloud, imgPoints, K, distortion_coeff, rvec, t, true, 100, 20.0F, 0.1 * (double)(imgPoints.size()), inliers, CV_EPNP);
	solvePnPRansac(ppcloud, imgPoints, K, distortion_coeff, rvec, t, true, 1000, reprojectionError, 0.7 * (double)(imgPoints.size()), inliers, CV_EPNP);
	//solvePnPRansac(ppcloud, imgPoints, K, distortion_coeff, rvec, t, true, 1000, reprojectionError, 0.9 * (double)(imgPoints.size()), inliers, CV_EPNP);
	double end=time.getElapsedTimeInMilliSec();

	cout<<"solve PnP Racsac time: "<<(end-start)<<endl;
	//solvePnP(ppcloud, imgPoints, K, distortion_coeff, rvec, t, true, CV_EPNP);

	//cout<<0.006 * maxVal<<endl;
	vector<Point2f> projected3D;
	projectPoints(ppcloud, rvec, t, K, distortion_coeff, projected3D);

	cout<<"-------------------------------------------------------------------------"<<endl;
	cout<<"find pose estimation"<<endl;
	cout<<"3d point: "<<ppcloud.size()<<endl;
	cout<<"2d point: "<<imgPoints.size()<<endl;
	cout<<"inliers size: "<<inliers.size()<<endl;
	/*for(int i=0;i<inliers.size();i++)
	{
		cout<<inliers[i]<<"  ";
	}
	cout<<endl;*/
	cout<<"projected point: "<<projected3D.size()<<endl;
	cout<<"--------------------------------------------------------------------------"<<endl;
	double err = 0.0;
	inliers.clear();
	if(inliers.size()==0) { //get inliers
		cout << "get inliers" << endl;
		cout << projected3D.size() << endl;
		for(int i=0;i<projected3D.size();i++) {
			if (norm(projected3D[i] - imgPoints[i]) < 10.0){
				err += norm(projected3D[i] - imgPoints[i]);
				inliers.push_back(i);
				if (isTriangulating)
				{
					Scalar c1(0, 0, 255);
					Scalar c2(255, 0, 0);
					render.FilledCircle(image, projected3D[i], c1);
					render.FilledCircle(image, imgPoints[i], c2);
					render.Line(image, projected3D[i], imgPoints[i]);
				}
			}
		}
	}
	err /= (double)inliers.size();
	projectError.push_back(err);
	cout << "after projection and compute distance inliers size is: " << inliers.size() << endl;


#if 0
	//display reprojected points and matches
	Mat reprojected; imgs_orig[working_view].copyTo(reprojected);
	for(int ppt=0;ppt<imgPoints.size();ppt++) {
		line(reprojected,imgPoints[ppt],projected3D[ppt],Scalar(0,0,255),1);
	}
	for (int ppt=0; ppt<inliers.size(); ppt++) {
		line(reprojected,imgPoints[inliers[ppt]],projected3D[inliers[ppt]],Scalar(0,0,255),1);
	}
	for(int ppt=0;ppt<imgPoints.size();ppt++) {
		circle(reprojected, imgPoints[ppt], 2, Scalar(255,0,0), CV_FILLED);
		circle(reprojected, projected3D[ppt], 2, Scalar(0,255,0), CV_FILLED);			
	}
	for (int ppt=0; ppt<inliers.size(); ppt++) {
		circle(reprojected, imgPoints[inliers[ppt]], 2, Scalar(255,255,0), CV_FILLED);
	}
	stringstream ss; ss << "inliers " << inliers.size() << " / " << projected3D.size();
	putText(reprojected, ss.str(), Point(5,20), CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(0,255,255), 2);

	imshow("__tmp", reprojected);
	waitKey(0);
	destroyWindow("__tmp");
#endif
	//Rodrigues(rvec, R);
	//visualizerShowCamera(R,t,0,255,0,0.1);
	cout<<"("<<inliers.size()<<"/"<<imgPoints.size()<<")"<< endl;
	if(inliers.size() < 10){//(double)(imgPoints.size())/5.0) {
		cout << "not enough inliers to consider a good pose ("<<inliers.size()<<"/"<<imgPoints.size()<<")"<< endl;
		return false;
	}

	if(norm(t) > 200.0) {
		// this is bad...
		cout << "estimated camera movement is too big, skip this camera\r\n";
		return false;
	}

	Rodrigues(rvec, R);
	if(!CheckCoherentRotation(R)) {
		cout << "rotation is incoherent. we should try a different base view..." << endl;
		return false;
	}

	cout << "found t = " << t << "\nR = \n"<<R<<endl;
	return true;
}




void CloudMap::GetRGBForPointCloud(
	const vector<CloudPoint>& _pcloud,
	vector<Vec3b>& RGBforCloud
	) 
{
	RGBforCloud.resize(_pcloud.size());
	for (unsigned int i=0; i<_pcloud.size(); i++) {
		unsigned int good_view = 0;
		vector<Vec3b> point_colors;
		for(; good_view < keyFrames.size(); good_view++) {
			if(_pcloud[i].imgpt_for_img[good_view] != -1) {
				int pt_idx = _pcloud[i].imgpt_for_img[good_view];
				if(pt_idx >= keyFrames[good_view].keyPoints.size()) {
					cerr << "BUG: point id:" << pt_idx << " should not exist for img #" << good_view << " which has only " << keyFrames[good_view].keyPoints.size() << endl;
					continue;
				}
				Point _pt = keyFrames[good_view].keyPoints[pt_idx].pt;
				assert(good_view < keyFrames.size() && _pt.x < keyFrames[good_view].image.cols && _pt.y < keyFrames[good_view].image.rows);
				
				point_colors.push_back(keyFrames[good_view].image.at<Vec3b>(_pt));
				
//				stringstream ss; ss << "patch " << good_view;
//				imshow_250x250(ss.str(), imgs_orig[good_view](Range(_pt.y-10,_pt.y+10),Range(_pt.x-10,_pt.x+10)));
			}
		}
//		waitKey(0);
		Scalar res_color = mean(point_colors);
		RGBforCloud[i] = (Vec3b(res_color[0],res_color[1],res_color[2])); //bgr2rgb
		if(good_view == keyFrames.size()) //nothing found.. put red dot
			RGBforCloud.push_back(Vec3b(255,0,0));
	}
}


vector<Vec3b>& CloudMap::getPointCloudRGB() 
{ 
	if(pointCloudRGB.size()<pcloud.size()) 
	{ 
		cout << "before get rgb, rgb size: " << pointCloudRGB.size() << " pcloud size: " << pcloud.size() << endl;
		GetRGBForPointCloud(pcloud,pointCloudRGB); 
		cout << "after get rgb, rgb size: " << pointCloudRGB.size() << " pcloud size: " << pcloud.size() << endl;
	} 
	return pointCloudRGB; 
}



Point3d CloudMap::ComputeCenter()
{
	double xall = 0;
	double yall = 0;
	double zall = 0;
	for (int i = 0; i<pcloud.size(); i++)
	{
		xall += pcloud[i].pt.x;
		yall += pcloud[i].pt.y;
		zall += pcloud[i].pt.z;
	}
	double x = xall / pcloud.size();
	double y = yall / pcloud.size();
	double z = zall / pcloud.size();
	ofstream centerfile;
	string out = path + "/out";
	centerfile.open(out+"/center.txt", ios::out);
	centerfile << 1 << endl;
	centerfile << x << "  " << y << "  " << z << "  " << endl;
	centerfile.flush();
	centerfile.close();
	//CloudPoint& cp = pcloud[pcloud.size() / 2];
	Point3d center(x, y, z);
	return center;
}

void CloudMap::writeMap()
{
	string out = path + "/out";
	for (int i = 0; i < keyFrames.size(); i++)
	{
		imwrite(out+"/"+intToString(i) + ".jpg", keyFrames[i].image);
	}



	fstream pmatfile(out+"/Pmats.txt", ios::out);
	pmatfile << keyFrames.size() << endl;
	for (int i = 0; i < keyFrames.size(); i++)
	{
		for (int j = 0; j < 12; j++)
		{
			pmatfile << keyFrames[i].Pmats.val[j] << "  ";
		}
		pmatfile << endl;
	}


	double xall = 0;
	double yall = 0;
	double zall = 0;
	for (int i = 0; i<pcloud.size(); i++)
	{
		xall += pcloud[i].pt.x;
		yall += pcloud[i].pt.y;
		zall += pcloud[i].pt.z;
	}
	double x = xall / pcloud.size();
	double y = yall / pcloud.size();
	double z = zall / pcloud.size();
	ofstream center;
	center.open(out + "/center.txt", ios::out);
	center << 1 << endl;
	center << x << "  " << y << "  " << z << "  " << endl;
	center.flush();
	center.close();
}


void CloudMap::fillPoseVal(float val[])
{
	val[0] = estimatedR.at<double>(0, 0);
	val[1] = estimatedR.at<double>(0, 1);
	val[2] = estimatedR.at<double>(0, 2);
	val[3] = estimatedT.at<double>(0, 0);

	val[4] = estimatedR.at<double>(1, 0);
	val[5] = estimatedR.at<double>(1, 1);
	val[6] = estimatedR.at<double>(1, 2);
	val[7] = estimatedT.at<double>(0, 1);

	val[8] = estimatedR.at<double>(2, 0);
	val[9] = estimatedR.at<double>(2, 1);
	val[10] = estimatedR.at<double>(2, 2);
	val[11] = estimatedT.at<double>(0, 2);
}

void CloudMap::writeError()
{
	path = "/sdcard/err.txt";
	fstream file;
	file.open(path, ios::out);
	double mean = 0.0;
	for (int i = 0; i < projectError.size(); i++)
	{
		mean += projectError[i];
	}
	mean /= (double)projectError.size();
	file << projectError.size() << "  " << mean << endl;
	for (int i = 0; i < projectError.size(); i++)
	{
		file << projectError[i] << endl;
	}
	file.close();
}



void CloudMap::ExtractFeaturesFromMatches(KeyFrame& queryFrame, KeyFrame& trainFrame, vector<DMatch>& matches)
{
	vector<KeyPoint> selectedQueryKeyPoints, selectedTrainKeyPoints;
	int rows = matches.size();
	int cols = queryFrame.descriptors.cols;
	int type = queryFrame.descriptors.type();
	Mat selectedQueryDescriptors(rows, cols, type), selectedTrainDescriptors(rows, cols, type);
	vector<DMatch> selectedMatches;
	for (int i = 0; i < matches.size(); i++)
	{
		//cout << "begin select keypoints" << endl;
		int queryIndex = matches[i].queryIdx;
		int trainIndex = matches[i].trainIdx;
		selectedQueryKeyPoints.push_back(queryFrame.keyPoints[queryIndex]);
		selectedQueryKeyPoints[selectedQueryKeyPoints.size() - 1].class_id = i;
		selectedTrainKeyPoints.push_back(trainFrame.keyPoints[trainIndex]);
		selectedTrainKeyPoints[selectedTrainKeyPoints.size() - 1].class_id = i;

		//cout << "begin select descriptors" << endl;
		//cout << "query frame col size: " << queryFrame.descriptors.cols << queryFrame.descriptors.rows << endl;
		//cout << queryFrame.descriptors.size() << endl;
		Mat queryDst = selectedQueryDescriptors.row(i);
		Mat querySrc = queryFrame.descriptors.row(queryIndex);
		querySrc.copyTo(queryDst);

		//cout << "train frame col size: " << trainFrame.descriptors.cols << trainFrame.descriptors.rows << endl;
		//cout << trainFrame.descriptors.size() << endl;
		Mat trainDst = selectedTrainDescriptors.row(i);
		Mat trainSrc = trainFrame.descriptors.row(trainIndex);
		trainSrc.copyTo(trainDst);


		//cout << "begin select matches" << endl;
		DMatch match(i, i, matches[i].distance);
		selectedMatches.push_back(match);
	}

	queryFrame.keyPoints.swap(selectedQueryKeyPoints);
	trainFrame.keyPoints.swap(selectedTrainKeyPoints);

	queryFrame.descriptors = selectedQueryDescriptors;
	trainFrame.descriptors = selectedTrainDescriptors;

	matches.swap(selectedMatches);
}



