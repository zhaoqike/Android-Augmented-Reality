package com.example.augmentedreality.ar;

public class ARNativeLib {
	
	static {
        System.loadLibrary("AugmentedReality");
    }
	
	
	//preprocess
	public static native void setFirstFlagNative();
	
	public static native void setSecondFlagNative();
	public static native void setStartPrepTrackFlagNative();
	public static native void storeMapNative();
	public static native int preprocessFrameNative(long matAddrGr, long matAddrRgba);
	
	
	//tracking
	public static native void setStartTrackFlagNative();
	public static native int trackingFrameNative(long matAddrGr, long matAddrRgba);
	public static native void loadMapNative();
	//common
	public static native void redirectStdOut();
	public static native void FindFeatures(long matAddrGr, long matAddrRgba);
	
	public static native int processFrameNative(long matAddrGr, long matAddrRgba);
	
	
	//test
	public static native int sendFrameToNative(long matAddrGr, long matAddrRgba);
	public static native void edgeDetectionNative();

	public static native boolean getGLPoseNative(float[] val);
	public static native boolean getCenterNative(float[] c);

	public static native void writeInfo();
}
