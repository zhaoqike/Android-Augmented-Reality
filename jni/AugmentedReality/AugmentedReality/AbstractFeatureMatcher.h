
#pragma once

#include "IFeatureMatcher.h"

class AbstractFeatureMatcher : public IFeatureMatcher {
protected:
	bool use_gpu;
public:
	AbstractFeatureMatcher(bool _use_gpu):use_gpu(_use_gpu) {}
};