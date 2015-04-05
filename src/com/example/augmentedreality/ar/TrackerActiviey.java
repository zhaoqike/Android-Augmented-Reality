package com.example.augmentedreality.ar;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import com.example.augmentedreality.ARActivity;
import com.example.augmentedreality.R;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.Toast;

public class TrackerActiviey extends ARActivity implements CvCameraViewListener2,State {

	private static final String TAG = "track activity";
	private Mat mRgba;
	private Mat mGray;
	private CameraBridgeViewBase mOpenCvCameraView;


	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i(TAG, "OpenCV loaded successfully");
				System.loadLibrary("AugmentedReality");
				mOpenCvCameraView.enableView();
				//mOpenCvCameraView.setOnTouchListener( MainActivity.this );
			} break;
			default:
			{
				super.onManagerConnected(status);
			} break;
			}
		}
	};


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.i(TAG, "called onCreate start");
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_main);

		/*if (savedInstanceState == null) {
			getFragmentManager().beginTransaction()
			.add(R.id.container, new PlaceholderFragment()).commit();
		}*/
		//Log.i(TAG, "called onCreatestart");

		getWindow().addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON );

		setContentView(R.layout.camera_view_layout);
		mOpenCvCameraView = (CameraBridgeViewBase) findViewById ( R.id.camera_view );
		if(mOpenCvCameraView==null)
		{
			Log.e(TAG, "mopencvcameraview is null");
		}
		mOpenCvCameraView.setVisibility( SurfaceView.VISIBLE );
		mOpenCvCameraView.setCvCameraViewListener(this);/**/
		//int height=mOpenCvCameraView.getMinimumHeight();
		//int width=mOpenCvCameraView.getMinimumWidth();
		//mOpenCvCameraView.setMaxFrameSize(width, height);
		Log.i(TAG, "called onCreate end");
		//Log.i(TAG,"width: "+Integer.toString(width)+"   height: "+Integer.toString(height));

		//redirectStdOut();
		//SysApplication.getInstance().addActivity(this); 
	}

	@Override
	public void onPause()
	{
		super.onPause();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	@Override
	public void onResume()
	{
		super.onResume();
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_9, this, mLoaderCallback);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		mOpenCvCameraView.disableView();
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		Log.e(TAG, "camera start");
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
		int state=ARNativeLib.processFrameNative(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
		if(state==gottenFirst)
		{
			showToast("first frame gotten");
		}
		else if(state==initFailed)
		{
			showToast("init map failed");
		}
		else if(state==initSuccess)
		{
			showToast("init map success");
		}
		else if(state==trackFailed)
		{
			showToast("tracking failed");
		}
		else if(state==triangulateFailed)
		{
			showToast("triangulation failed");
		}
        //FindFeatures(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
        return mRgba;
	}
	
	private void showToast(String toastMesage)
	{
		Toast toast = Toast.makeText(this, toastMesage, Toast.LENGTH_LONG);
		toast.show();
	}

}
