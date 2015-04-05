#include "StdAfx.h"
#include "PreProcessor.h"
#include "KeyFrame.h"
#include <fstream>

using namespace std;


PreProcessor::PreProcessor(void)
{
	getFirst=getSecond=startTracking=false;
}

PreProcessor::PreProcessor(bool flag)
{
	getFirst=getSecond=startTracking=false;
}

PreProcessor::~PreProcessor(void)
{
}


int PreProcessor::Run(void)
{
	cout<<"preprocessor begin"<<endl;
	VideoCapture capture;
    capture.open(1);

    if (!capture.isOpened())
    {

        cout << "capture device 1 failed to open!" << endl;
        return 1;
    }

    cout << "camera open successfully"<<endl;
	cout<<"getFirst  "<<getFirst<<endl;
	cout<<"getSecond  "<<getSecond<<endl;

    Mat frame;


	//set camera
	capture >> frame;
	if (frame.empty())
	{
		cout<<"frame is empty"<<endl;
        return 0;
	}
	map.SetCamera(frame.size());

	cout<<"Frame size: "<<frame.size().width<<"  "<<frame.size().height<<endl;

    
    for (;;)
    {
        capture >> frame;
		if (frame.empty())
		{
			cout<<"frame is empty"<<endl;
            break;
		}
		//cout<<"capture a frame"<<endl;
        ProcessFrame(frame);

		imshow("frame",frame);
		char key = (char)waitKey(2);
		switch (key)
        {
        case 'f':
            setFirstFlag();
            break;
        case 's':
            setSecondFlag();
            break;
		case 't':
			setTrackingFlag();
			break;
        case 27:
        case 'q':
			map.getPointCloudRGB();
			cout<<map.pcloud.size()<<"  "<<map.pointCloudRGB.size()<<endl;
			if(map.pointCloudRGB.size()>=map.pcloud.size())
			{
				map.writeCloud();
			}
            return 0;
            break;
        }
		
	}
	
    return 0;
}


PC PreProcessor::getAll(vector<Mat>& images,int i,int j)
{
	map.keyFrames.clear();
	map.pcloud.clear();
	map.MakeKeyFrame(images[i]);
	map.MakeKeyFrame(images[j]);
	map.InitMap();

	PC pc;
	pc.i=i;
	pc.j=j;
	pc.count=map.pcloud.size();
	pc.P=map.keyFrames[0].Pmats;
	pc.P1=map.keyFrames[1].Pmats;
	pc.R=Matx33d(pc.P(0,0),pc.P(0,1),pc.P(0,2),
				pc.P(1,0),pc.P(1,1),pc.P(1,2),
				pc.P(2,0),pc.P(2,1),pc.P(2,2));
	pc.R1=Matx33d(pc.P1(0,0),pc.P1(0,1),pc.P1(0,2),
				pc.P1(1,0),pc.P1(1,1),pc.P1(1,2),
				pc.P1(2,0),pc.P1(2,1),pc.P1(2,2));
	invert(pc.R,pc.Rinv);
	invert(pc.R1,pc.R1inv);
	return pc;
}

int PreProcessor::Test(string path)
{
	CloudMap map;
	redirectStdout(path);
	//redirectStderr(path);

	//freopen(filename.c_str(),"w",stdout);
	//freopen("out.txt","w",stderr);
	cout<<"hello world"<<endl;
	vector<string> images_names;
	vector<Mat> images;

	open_imgs_dir(path,images,images_names,1.0);
	for(int i=0;i<images_names.size();i++)
	{
		cout<<images_names[i]<<endl;
	}
	map.path = path;
	map.SetCamera(images[0].size());

	map.MakeKeyFrame(images[0]);
	cout<<"keyframe 1 end"<<endl;
	cerr<<"keyframe 1 end"<<endl;
	cerr << map.keyFrames[0].descriptors.type();
	map.MakeKeyFrame(images[1]);
	cout<<"keyframe 2 end"<<endl;
	cerr<<"keyframe 2 end"<<endl;

	cerr<<"begin init map"<<endl;
	cerr<<"keyframe 0: "<<map.keyFrames[0].keyPoints.size()<<"  "<<map.keyFrames[0].keyPointsGood.size()<<endl;
	cerr<<"keyframe 1: "<<map.keyFrames[1].keyPoints.size()<<"  "<<map.keyFrames[1].keyPointsGood.size()<<endl;
	map.InitMap();
	cerr<<"end init map"<<endl;
	cout<<"after init map size: "<<map.pcloud.size()<<endl;
	if (map.pcloud.size() == 0)
	{
		cerr << path << "init failed" << endl;
		return -1;
	}
	for(int i=2;i<images.size();i++)
	{
		cout << "begin track " + intToString(i) + " map" << endl;
		map.TrackFrame(images[i]);
		cout<<"after " + intToString(i)+ " map size: "<<map.pcloud.size()<<endl;
	}

	map.StoreMap(mapPath);

	cout<<"get color: "<<endl;
	map.getPointCloudRGB();
	cout<<map.pcloud.size()<<"  "<<map.pointCloudRGB.size()<<endl;
	if(map.pointCloudRGB.size()>=map.pcloud.size())
	{
		cerr << "write cloud" << endl;
		map.writeCloud();
	}
	cout << "begin write pmats" << endl;
	//map.writePmats();
	//map.ComputeCenter();
	for (int i = 0; i < images.size(); i++)
	{
		imwrite(path + "/out/" + intToString(i)+".jpg",images[i]);
	}

	cerr << "end write pmats" << endl;
	//map.writeMap();
	return 0;
}


int PreProcessor::ProcessFrame(Mat &frame)
{
	if (frame.empty())
	{
		cout<<"frame empty"<<endl;
		return -1;
	}
	if(getFirst==false)
	{
		//cerr<<"fenzhi 1"<<endl;
		KeyFrame keyframe;
		map.MakeKeyFrameLite(keyframe,frame);
		//cout<<"keypoints num: "<<keyframe->keyPoints.size()<<endl;
		drawKeypoints(frame, keyframe.keyPoints, frame, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_OVER_OUTIMG);
		return beforeFirst;
	}
	else if(getFirst==true && map.NeedFirst())
	{
		cerr<<"fenzhi 2"<<endl;
		map.SetCamera(frame.size());
		//KeyFrame keyframe;
		map.MakeKeyFrame(frame);
		KeyFrame keyframe=map.keyFrames[0];
		drawKeypoints(frame, keyframe.keyPoints, frame, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_OVER_OUTIMG);

		cout<<"create first keyframe"<<endl;
		cout<<"keyframe number: "<<map.keyFrames.size()<<endl;
		cout<<"first keyframe keypoints number: "<<map.keyFrames[0].keyPoints.size();
		return gottenFirst;
	}
	else if(getSecond==false)
	{
		//cerr<<"fenzhi 3"<<endl;
		KeyFrame keyframe;
		map.MakeKeyFrameLite(keyframe,frame);


		KeyFrame& trainframe=map.keyFrames.front();
		vector<KeyPoint> query_kpts=keyframe.keyPoints;
		vector<KeyPoint> train_kpts=trainframe.keyPoints;
		vector<DMatch> matches;
		FeatureMatcher matcher;
		matcher.Match(keyframe,trainframe,matches);
		drawMatchesRelative(query_kpts, train_kpts, matches, frame);
		return beforeSecond;
	}
	else if(getSecond==true && map.NeedSecond())
	{
		cerr<<"fenzhi 4"<<endl;
		
		map.MakeKeyFrame(frame);


		KeyFrame& trainframe=map.keyFrames.front();
		KeyFrame& keyframe=map.keyFrames[1];
		vector<KeyPoint> query_kpts=keyframe.keyPoints;
		vector<KeyPoint> train_kpts=trainframe.keyPoints;
		vector<DMatch> matches;
		FeatureMatcher matcher;
		matcher.Match(keyframe,trainframe,matches);
		drawMatchesRelative(query_kpts, train_kpts, matches, frame);


		map.InitMap();

		cout<<"create second keyframe"<<endl;
		cout<<"create init map"<<endl;
		cout<<"keyframe number: "<<map.keyFrames.size()<<endl;
		cout<<"second keyframe keypoints number: "<<map.keyFrames[1].keyPoints.size()<<endl;
		cout<<"point number: "<<map.pcloud.size()<<endl;
		map.keyFrames[0].countIndex();
		map.keyFrames[1].countIndex();

		imwrite("keyframe1.jpg", map.keyFrames[0].image);
		imwrite("keyframe2.jpg", map.keyFrames[1].image);
		if (map.pcloud.size() == 0)
		{
			//set to init
			getFirst = false;
			getSecond = false;
			map.keyFrames.clear();
			map.pcloud.clear();
			cout << "init failed" << endl;

			return initFailed;
		}
		else
		{
			cout << "init success" << endl;
			return initSuccess;
		}
	}
		
	else if(startTracking==false)
	{
		//cout<<"fenzhi 5"<<endl;
		return beforeTrack;
	}
	else
	{
		cerr<<"fenzhi 6"<<endl;
		int result=map.TrackFrame(frame);
		return result;
	}
}

int PreProcessor::setFirstFlag()
{
	getFirst=true;
	cout<<"get first is true now"<<endl;
	return 0;
}

int PreProcessor::setSecondFlag()
{
	getSecond=true;
	cout<<"get second is true now"<<endl;
	return 0;
}

int PreProcessor::setTrackingFlag()
{
	startTracking=true;
	cout<<"start tracking is true now"<<endl;
	return 0;
}










































void PreProcessor::drawMatchesRelative(const vector<KeyPoint>& query, const vector<KeyPoint>& train,
    vector<DMatch>& matches, Mat& img)
{
    for (int i = 0; i < (int)matches.size(); i++)
    {
        
        Point2f pt_new = query[matches[i].queryIdx].pt;
        Point2f pt_old = train[matches[i].trainIdx].pt;

        line(img, pt_new, pt_old, Scalar(125, 255, 125), 1);
        circle(img, pt_new, 2, Scalar(255, 0, 125), 1);

        
    }
}

//Takes a descriptor and turns it into an xy point
void PreProcessor::keypoints2points(const vector<KeyPoint>& in, vector<Point2f>& out)
{
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i)
    {
        out.push_back(in[i].pt);
    }
}

//Takes an xy point and appends that to a keypoint structure
void PreProcessor::points2keypoints(const vector<Point2f>& in, vector<KeyPoint>& out)
{
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i)
    {
        out.push_back(KeyPoint(in[i], 1));
    }
}

//Uses computed homography H to warp original input points to new planar position
void PreProcessor::warpKeypoints(const Mat& H, const vector<KeyPoint>& in, vector<KeyPoint>& out)
{
    vector<Point2f> pts;
    keypoints2points(in, pts);
    vector<Point2f> pts_w(pts.size());
    Mat m_pts_w(pts_w);
    perspectiveTransform(Mat(pts), m_pts_w, H);
    points2keypoints(pts_w, out);
}

//Converts matching indices to xy points
void PreProcessor::matches2points(const vector<KeyPoint>& train, const vector<KeyPoint>& query,
    const vector<DMatch>& matches, vector<Point2f>& pts_train,
    vector<Point2f>& pts_query)
{

    pts_train.clear();
    pts_query.clear();
    pts_train.reserve(matches.size());
    pts_query.reserve(matches.size());

    size_t i = 0;

    for (; i < matches.size(); i++)
    {

        const DMatch & dmatch = matches[i];

        pts_query.push_back(query[dmatch.queryIdx].pt);
        pts_train.push_back(train[dmatch.trainIdx].pt);

    }

}

void PreProcessor::resetH(Mat&H)
{
    H = Mat::eye(3, 3, CV_32FC1);
}
