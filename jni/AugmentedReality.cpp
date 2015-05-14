#include <jni.h>

#include "fstream"


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "Map.h"
#include "PreProcessor.h"
#include "Tracker.h"
#include <thread>




using namespace std;
using namespace cv;

Map map1;
bool isRedirect=false;
PreProcessor preprocessor(false);
Tracker tracker;
Mat rgb;
Mat gray;
Mutex rgblock;
bool obtainNewFrame=false;

bool trackState=false;
float val[12];
Point3d center;

Mat processRgb;
Mutex processRgbLock;
bool processNewFrame=false;


extern "C" {
//preprocess
JNIEXPORT jint JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_preprocessFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setFirstFlagNative(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setSecondFlagNative(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_storeMapNative(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setStartPrepTrackFlagNative(JNIEnv*, jobject);

//tracking
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setStartTrackFlagNative(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_loadMapNative(JNIEnv*, jobject);
JNIEXPORT jint JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_trackingFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_writeInfo(JNIEnv*, jobject);


//getpose
JNIEXPORT jboolean JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_getGLPoseNative(JNIEnv* env, jobject, jfloatArray arr);
JNIEXPORT jboolean JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_getCenterNative(JNIEnv* env, jobject, jfloatArray arr);




//common
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_sendFrameToNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_edgeDetectionNative(JNIEnv*, jobject);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_processFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_redirectStdOut(JNIEnv*, jobject);
}
//declare
void redirectStdOut();


//preprocess
JNIEXPORT jint JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_preprocessFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
	Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	int state = preprocessor.ProcessFrame(mRgb);
	if(state==trackFailed)
	{
		//trackState=false;
	}
	else if(state==trackSuccess)
	{
		trackState=true;
		preprocessor.map.fillPoseVal(val);
		cout<<"we have a success track"<<endl;
		for(int i=0;i<12;i++)
		{
			cout<<val[i]<<endl;
		}
	}
	else if(state==initSuccess)
	{
		center=preprocessor.map.ComputeCenter();
		cout<<"now we have center"<<endl;
		cout<<center.x<<"  "<<center.y<<"  "<<center.z<<endl;
		Model model("/sdcard/ar/bj1/models/cow.ply");
		model.p3d = center;
		preprocessor.map.model=model;
	}
	else if(state==triangulateSuccess)
	{
		cout<<"now we add a new keyframe"<<endl;
		cout<<"keyframes size: "<<preprocessor.map.keyFrames.size()<<endl;
		cout<<"cloud map size: "<<preprocessor.map.pcloud.size()<<endl;
	}
	return state;
}

JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setFirstFlagNative(JNIEnv*, jobject){
	//map1.changeColor();
	//preprocessor.Run();
	redirectStdOut();
	preprocessor.setFirstFlag();

}

JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setSecondFlagNative(JNIEnv*, jobject)
{
	preprocessor.setSecondFlag();
}
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_storeMapNative(JNIEnv*, jobject)
{
	preprocessor.map.StoreMap("/sdcard/map.yml");
	preprocessor.map.writeError();
}

JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setStartPrepTrackFlagNative(JNIEnv*, jobject)
{
	preprocessor.setTrackingFlag();
}

//tracker
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_setStartTrackFlagNative(JNIEnv*, jobject)
{
	tracker.setTrackingFlag();
}
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_loadMapNative(JNIEnv*, jobject)
{
	redirectStdOut();
	tracker.cloudmap.LoadMap("/sdcard/map.yml");
	tracker.cloudmap.StoreMap("/sdcard/map1.yml");
	cout<<"tracker"<<endl;
	cout<<tracker.cloudmap.keyFrames.size()<<endl;
	cout<<tracker.cloudmap.pcloud.size()<<endl;
	center=tracker.cloudmap.ComputeCenter();
	cout<<"now we have center"<<endl;
	cout<<center.x<<"  "<<center.y<<"  "<<center.z<<endl;
	Model model("/sdcard/ar/bj1/models/cow.ply");
	model.p3d = center;
	tracker.cloudmap.model=model;
}
JNIEXPORT jint JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_trackingFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
	double nowglbtime=gtimer.getElapsedTimeInMilliSec();
	double glbduration=nowglbtime-lastglbtime;
	lastglbtime=nowglbtime;
	frames.push_back(1000.0/glbduration);
	Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	int state=tracker.TrackFrame(mRgb);
	if(state==trackFailed)
	{
		trackState=false;
	}
	else if(state==trackSuccess)
	{
		trackState=true;
		tracker.cloudmap.fillPoseVal(val);
	}
	return state;
}

void printFrames()
{
	ofstream framefile;
	framefile.open("sdcard/frames3d.txt");
	framefile<<frames.size()<<endl;
	for(int i=0;i<frames.size();i++)
	{
		framefile<<frames[i]<<endl;
	}
	framefile.flush();
	framefile.close();
}

JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_writeInfo(JNIEnv*, jobject)
{
	printFrames();
	tracker.cloudmap.writeError();
}


//getpose
JNIEXPORT jboolean JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_getGLPoseNative(JNIEnv* env, jobject, jfloatArray arr)
{
	jfloat *carr;
	carr = env->GetFloatArrayElements(arr, false);   //获得Java数组arr的引用的指针
	if(carr == 0) {
		cout<<"get gl pose native ptr is null"<<endl;
		return 0; /* exception occurred */
	}
	//jfloat sum = 0;
	jsize len = (env)->GetArrayLength(arr);
	if(len!=12)
	{
		cout<<"gl pose native len is not 12"<<endl;
		return trackState;
	}
	for(int i=0; i<len; i++) {
		carr[i]=val[i];
		//sum += carr[i];
	}
	env->ReleaseFloatArrayElements(arr, carr, 0);
	return trackState;
}


JNIEXPORT jboolean JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_getCenterNative(JNIEnv* env, jobject, jfloatArray arr)
{
	jfloat *carr;
	carr = env->GetFloatArrayElements(arr, false);   //获得Java数组arr的引用的指针
	if(carr == 0) {
		cout<<"get center native ptr is null"<<endl;
		return 0; /* exception occurred */
	}

	//jfloat sum = 0;
	jsize len = (env)->GetArrayLength(arr);
	if(len!=3)
	{
		cout<<"get center native len is not 3"<<endl;
	}
	//cout<<"now we copy center"<<endl;
	//cout<<"before: "<<carr[0]<<"  "<<carr[1]<<"  "<<carr[2]<<"  "<<endl;
	carr[0]=(float)center.x;
	carr[1]=(float)center.y;
	carr[2]=(float)center.z;
	//cout<<"after: "<<carr[0]<<"  "<<carr[1]<<"  "<<carr[2]<<"  "<<endl;
	env->ReleaseFloatArrayElements(arr, carr, 0);
	return trackState;
}


//common
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_redirectStdOut(JNIEnv*, jobject)
{
	redirectStdOut();
}


JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_processFrameNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
	Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	vector<KeyPoint> v;

	FastFeatureDetector detector(50);
	detector.detect(mGr, v);
	for( unsigned int i = 0; i < v.size(); i++ )
	{
		const KeyPoint& kp = v[i];
		circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
	}
}
void readwrite()
{
	Mat image;
	image = cv::imread("abcd.bmp");
	cv::imwrite("aaaaa.bmp", image);
}

void redirectStdOut()
{
	if(isRedirect==true)
	{
		return;
	}
	freopen("/sdcard/arStdout.txt","w",stdout);
	cout<<"this is ar std out"<<endl;
	isRedirect=true;
}


JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_sendFrameToNative(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
	Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	vector<KeyPoint> v;

	FastFeatureDetector detector(50);
	detector.detect(mGr, v);
	for( unsigned int i = 0; i < v.size(); i++ )
	{
		const KeyPoint& kp = v[i];
		circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
	}
	/*Mat& mGr  = *(Mat*)addrGray;
	Mat& mRgb = *(Mat*)addrRgba;
	Sobel(mRgb,mRgb,mRgb.depth(),1,1);
	//Mat dst;
	//Laplacian(mRgb,dst,mRgb.depth());
	//mRgb=dst.clone();
	return;
	rgblock.lock();
	rgb=mRgb.clone();
	//gray=mGr.clone();
	obtainNewFrame=true;
	rgblock.unlock();

	processRgbLock.lock();
	if(processNewFrame==true)
	{
		mRgb=processRgb.clone();
		processNewFrame=false;
	}
	processRgbLock.unlock();*/
}


void edgeDetection()
{
	while(true)
	{
		Mat src;
		rgblock.lock();
		if(obtainNewFrame==false)
		{
			rgblock.unlock();
			return;
		}
		src=rgb.clone();
		obtainNewFrame=false;

		rgblock.unlock();

		Mat dst;
		Sobel(src,dst,src.depth(),1,1);

		processRgbLock.lock();
		processRgb=dst.clone();
		processNewFrame=true;
		processRgbLock.unlock();

		//sleep(20);
	}
}
JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_edgeDetectionNative(JNIEnv*, jobject)
{
	edgeDetection();
}

JNIEXPORT void JNICALL Java_com_example_augmentedreality_ar_ARNativeLib_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
	/*Mat& mGr  = *(Mat*)addrGray;
    Mat& mRgb = *(Mat*)addrRgba;
    vector<KeyPoint> v;

    FastFeatureDetector detector(50);
    detector.detect(mGr, v);
    for( unsigned int i = 0; i < v.size(); i++ )
    {
        const KeyPoint& kp = v[i];
        circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
    }*/
	//redirectStdOut();
	//map1.findfeature((long long)addrGray,(long long)addrRgba);
	//map1.test();
	usleep(1000*30);
}



