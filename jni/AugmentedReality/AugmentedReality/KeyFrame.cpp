#include "StdAfx.h"
#include "KeyFrame.h"
#include "YamlStorage.h"

KeyFrame::KeyFrame(void)
{
}


KeyFrame::~KeyFrame(void)
{
}

int KeyFrame::countIndex()
{
	int size=indexInMap.size();
	int count=0;
	for(int i=0;i<indexInMap.size();i++)
	{
		if(indexInMap[i]!=-1)
		{
			count++;
		}
	}
	cout<<count<<"/"<<size<<endl;
	return count;
}

//KeyFrame KeyFrame::makeKeyFrame(Mat img)
//{
//	image=img.clone();
//	ORB orb;
//	orb(image, Mat(), keyPoints, descriptors);
//	return this;
//}

void KeyPointsToMyKeyPoints(const vector<KeyPoint>& keypoints, vector<MyKeyPoint>& mykeypoints)
{
	mykeypoints.clear();
	mykeypoints.reserve(keypoints.size());
	for (int i = 0; i < keypoints.size(); i++)
	{
		mykeypoints.push_back(MyKeyPoint(keypoints[i]));
	}
}

void MykeyPointsToKeyPoints(vector<MyKeyPoint>& mykeypoints, vector<KeyPoint>& keypoints)
{
	keypoints.clear();
	keypoints.reserve(mykeypoints.size());
	for (int i = 0; i < mykeypoints.size(); i++)
	{
		keypoints.push_back(mykeypoints[i].kpt);
	}
}

void KeyFrame::write(FileStorage& fs) const
{
	vector<MyKeyPoint> mykeypoints;
	KeyPointsToMyKeyPoints(keyPoints, mykeypoints);

	vector<MyKeyPoint> mykeypointsgood;
	KeyPointsToMyKeyPoints(keyPointsGood, mykeypointsgood);

	fs << "{" 
		<< "kpts" << mykeypoints 
		<< "kptsgood" << mykeypointsgood
		<< "index" << indexInMap 
		<< "descriptor" << descriptors 
		<< "pmat" << (Mat)Pmats 
		<< "}";
}
void KeyFrame::read(const FileNode& node)
{
	vector<MyKeyPoint> mykeypoints;
	vector<MyKeyPoint> mykeypointsgood;
	node["kpts"] >> mykeypoints;
	node["kptsgood"] >> mykeypointsgood;
	MykeyPointsToKeyPoints(mykeypoints, keyPoints);
	MykeyPointsToKeyPoints(mykeypointsgood, keyPointsGood);
	node["index"] >> indexInMap;
	node["descriptor"] >> descriptors;
	Mat p;
	node["pmat"] >> p;
	Pmats = Matx34d(p);
}
