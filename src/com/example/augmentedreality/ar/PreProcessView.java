package com.example.augmentedreality.ar;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

public class PreProcessView implements CvCameraViewListener2{
	
	private CameraBridgeViewBase mOpenCvCameraView;
	private Mat mRgba;
    private Mat mGray;

	@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		mGray = new Mat();
        mRgba = new Mat();
	}

	@Override
	public void onCameraViewStopped() {
		// TODO Auto-generated method stub
		mRgba.release();
		mGray.release();
	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		// TODO Auto-generated method stub
		mRgba = inputFrame.rgba();
		mGray = inputFrame.gray();
		return mGray;
	}

}
