#include "StdAfx.h"
#include "Tracker.h"


Tracker::Tracker(void)
{
	isFirst=true;
	startTracking = false;
	isSetCamera = false;
}


Tracker::~Tracker(void)
{
}


void Tracker::TrackForInitialMap(void)
{
}


void Tracker::Run(void)
{
	Load3DModel();
	if(isFirst)
	{
		isFirst=false;
	}

}


void Tracker::Load3DModel()
{
	cloudmap.LoadMap(mapPath);
	cout<<"tracker"<<endl;
	cout<<cloudmap.keyFrames.size()<<endl;
	cout<<cloudmap.pcloud.size()<<endl;
}

int Tracker::TrackFrame(Mat& frame)
{
	if (isSetCamera == false)
	{
		cloudmap.SetCamera(frame.size());
		isSetCamera = true;
	}
	if (startTracking)
	{
		int result = cloudmap.TrackFrame(frame);
		return result;
	}
	return 0;
}

int Tracker::setTrackingFlag()
{
	startTracking = true;
	cout << "start tracking is true now" << endl;
	return 0;
}
