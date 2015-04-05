

#include "IFeatureMatcher.h"

class RichFeatureMatcher : public IFeatureMatcher {
private:
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	
	vector<Mat> descriptors;
	
	vector<Mat>& imgs; 
	vector<vector<KeyPoint> >& imgpts;
public:
	//c'tor
	RichFeatureMatcher(vector<Mat>& imgs, 
					   vector<vector<KeyPoint> >& imgpts);
	
	void MatchFeatures(int idx_i, int idx_j, vector<DMatch>* matches = NULL);
	
	vector<KeyPoint> GetImagePoints(int idx) { return imgpts[idx]; }
};
