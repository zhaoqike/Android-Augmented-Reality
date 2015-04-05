

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

#include "IDistance.h"

#include "Common.h"


/**
 Feature Matching Interface
 */
class IFeatureMatcher {
public:
	virtual void MatchFeatures(int idx_i, int idx_j, vector<DMatch>* matches) = 0;
	virtual vector<KeyPoint> GetImagePoints(int idx) = 0;
};
