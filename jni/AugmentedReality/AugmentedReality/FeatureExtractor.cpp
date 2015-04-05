#include "StdAfx.h"
#include "FeatureExtractor.h"


FeatureExtractor::FeatureExtractor(void)
{
}

//FeatureExtractor::FeatureExtractor(int threshold)
//{
//	this->threshold=threshold;
//	
//}

FeatureExtractor::FeatureExtractor(int num)
{
	this->num=num;
	
}

FeatureExtractor::~FeatureExtractor(void)
{
}


void FeatureExtractor::SetParameters()
{

}
int FeatureExtractor::ExtractFeatures(KeyFrame &keyframe, Mat &image)
{
	//detector = FeatureDetector::create("FAST");
	//extractor = DescriptorExtractor::create("ORB");
	

	detector = new FastFeatureDetector();// FeatureDetector::create("PyramidFAST");
	extractor = new BriefDescriptorExtractor(); //DescriptorExtractor::create("ORB");



	//extractor = new BRISK();
	
	//cout << " -------------------- extract feature points for all images -------------------\n";
	Timer timer;
	timer.start();
	double detectstart = timer.getElapsedTimeInMilliSec();
	detector->detect(image, keyframe.keyPoints);
	double detectend = timer.getElapsedTimeInMilliSec();
	double detecttime = detectend - detectstart;
	cout << "detect keypoints cost: " << detecttime << endl;
	cout << "num is : " << num << endl;
	cout << "before retain keypoints is: "<<keyframe.keyPoints.size() << endl;
	if(keyframe.keyPoints.size()>num)
	{
		KeyPointsFilter filter;
		filter.retainBest(keyframe.keyPoints,num);
		//keyframe.keyPoints.resize(num);
	}
	cout << "after retain keypoints is: " << keyframe.keyPoints.size() << endl;
	double extractorstart = timer.getElapsedTimeInMilliSec();
	extractor->compute(image, keyframe.keyPoints, keyframe.descriptors);
	double extractorend = timer.getElapsedTimeInMilliSec();
	double extractortime = extractorend - extractorstart;
	cout << "extractor keypoints cost: " << extractortime << endl;
	//cout << " ------------------------------------- done -----------------------------------\n";

	//ORB orb(50000);
	//orb(image, Mat(), keyframe.keyPoints, keyframe.descriptors);

	return 0;
}