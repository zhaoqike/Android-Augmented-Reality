package com.example.augmentedreality.ar;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import android.content.Context;
import android.view.View;

public class CameraService extends View implements CvCameraViewListener2{


	public CameraService(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onCameraViewStopped() {
		// TODO Auto-generated method stub

	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		// TODO Auto-generated method stub
		return null;
	}

}
