#include "stdafx.h"
#include "MyKeyPoint.h"


MyKeyPoint::MyKeyPoint()
{
}
MyKeyPoint::MyKeyPoint(KeyPoint k)
{
	kpt = k;
}

MyKeyPoint::~MyKeyPoint()
{
}

void MyKeyPoint::write(FileStorage& fs) const
{
	//fs << "{"
	//	<< "i"<<i
	//	<< "}";

	fs << "{"
		<< "angle" << kpt.angle
		<< "id" << kpt.class_id
		<< "octive" << kpt.octave
		<< "pt" << kpt.pt
		<< "response"<<kpt.response
		<< "size"<<kpt.size
		<< "}";
}
void MyKeyPoint::read(const FileNode& node)
{
	//node["i"] >> i;
	node["angle"] >> kpt.angle;
	node["id"] >> kpt.class_id;
	node["octive"] >> kpt.octave;
	node["pt"] >> kpt.pt;
	node["response"] >> kpt.response;
	node["size"] >> kpt.size;
}