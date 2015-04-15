#pragma once
class CloudPoint
{
public:
	CloudPoint();
	~CloudPoint();


	Point3d pt;
	vector<int> imgpt_for_img;
	double reprojection_error;
	//Matx<uchar, 1, descriptorLen> descriptor;
	//Mat_<int> descriptor = (Mat_<int>(32, 1));
	Mat descriptor;

	void write(FileStorage& fs) const;

	void read(const FileNode& node);
};

