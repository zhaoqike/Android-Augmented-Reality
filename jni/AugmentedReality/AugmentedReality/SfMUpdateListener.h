#pragma once

#include <opencv2/core/core.hpp>

class SfMUpdateListener
{
public:
	virtual void update(vector<Point3d> pcld,
						vector<Vec3b> pcldrgb, 
						vector<Point3d> pcld_alternate,
						vector<Vec3b> pcldrgb_alternate, 
						vector<Matx34d> cameras) = 0;
};