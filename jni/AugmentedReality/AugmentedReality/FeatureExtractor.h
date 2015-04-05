#pragma once
class FeatureExtractor
{
public:
	int threshold;
	int num;
	
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;

	FeatureExtractor(void);
	//FeatureExtractor(int threshold);
	FeatureExtractor(int num);
	~FeatureExtractor(void);
	void SetParameters();
	int ExtractFeatures(KeyFrame &keyframe, Mat &image);
	
};

