#include "stdafx.h"
#include "CloudPoint.h"


CloudPoint::CloudPoint()
{
}


CloudPoint::~CloudPoint()
{
}

void CloudPoint::write(FileStorage& fs) const
{
	fs << "{" << "pt" << pt << "index" << imgpt_for_img << "error" << reprojection_error << "descriptor" << descriptor << "}";
}
void CloudPoint::read(const FileNode& node)
{
	node["pt"] >> pt;
	node["index"] >> imgpt_for_img;
	node["error"] >> reprojection_error;
	node["descriptor"] >> descriptor;
}