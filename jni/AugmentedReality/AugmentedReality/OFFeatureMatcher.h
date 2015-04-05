

#include "AbstractFeatureMatcher.h"

class OFFeatureMatcher : public AbstractFeatureMatcher {
	vector<Mat>& imgs; 
	vector<vector<KeyPoint> >& imgpts;
	
public:
	OFFeatureMatcher(bool _use_gpu,
					vector<Mat>& imgs_, 
					 vector<vector<KeyPoint> >& imgpts_);
	void MatchFeatures(int idx_i, int idx_j, vector<DMatch>* matches);
	vector<KeyPoint> GetImagePoints(int idx) { return imgpts[idx]; }
};
