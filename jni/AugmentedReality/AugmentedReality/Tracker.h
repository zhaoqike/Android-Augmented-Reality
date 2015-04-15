#pragma once
class Tracker
{
public:
	CloudMap cloudmap;
	bool isFirst;
	bool startTracking;
	bool isSetCamera;
	Tracker(void);
	~Tracker(void);
	void TrackForInitialMap(void);
	void Run(void);
	void Load3DModel();
	int TrackFrame(Mat& frame);
	int setTrackingFlag();

};

