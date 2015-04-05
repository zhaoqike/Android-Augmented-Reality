#pragma once
class MapPoint
{
public:

	Point3d pt;
	vector<int> imgpt_for_img;
	double reprojection_error;


	MapPoint(void);
	~MapPoint(void);
};

