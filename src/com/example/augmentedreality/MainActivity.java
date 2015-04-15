package com.example.augmentedreality;

import com.example.augmentedreality.ar.ARNativeLib;
import com.example.augmentedreality.ar.PreProcessActivity;
import com.example.augmentedreality.ar.State;
import com.example.augmentedreality.ar.SysApplication;
import com.example.augmentedreality.ar.TrackingActivity;
import com.example.augmentedreality.ar.TrackerActiviey;
import com.example.augmentedreality.camera.CameraActivity;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.objdetect.CascadeClassifier;











import android.R.bool;
import android.R.raw;
import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Toast;
import android.os.Build;
import android.util.Log;

public class MainActivity extends ARActivity  implements CvCameraViewListener2,State {
	
	
	private static final String TAG = "Main Activity";
	
	private final int preprocessMode=1;
	private final int trackMode=2;
	private int mode=0;
    private Mat mRgba;
    private Mat mGray;
    private CameraBridgeViewBase mOpenCvCameraView;
    
    /*SubMenu preprocessSubMenu=null;
    SubMenu trackerSubMenu=null;
    
    final int PRE_FIRST=1;
    final int PRE_SECOND=2;
    final int PRE_START=3;
    final int PRE_STORE=4;
    
    final int TRK_LOD=5;
    final int TRK_START=6;*/
    
    
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
    
    
    public MainActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
        Log.i(TAG, "Instantiated new " + this.getClass()+"end");
    }


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
        else{
        	Log.e(TAG,"mopencvcameraview not null");
        }

        mOpenCvCameraView.setVisibility( SurfaceView.VISIBLE );
        //mOpenCvCameraView.setResolution(640,480);
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
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		/*preprocessSubMenu=menu.addSubMenu("preprocess");
		preprocessSubMenu.add(0,PRE_FIRST,0,"first frame");
		preprocessSubMenu.add(0,PRE_SECOND,0,"second frame");
		preprocessSubMenu.add(0,PRE_START,0,"start tracker");
		preprocessSubMenu.add(0,PRE_STORE,0,"store map");
		trackerSubMenu=menu.addSubMenu("tracker");
		trackerSubMenu.add(0,TRK_LOD,0,"load map");
		trackerSubMenu.add(0,TRK_START,0,"start tracker");*/
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		/*int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}*/
		switch(item.getItemId())
		{
		case R.id.is_preprocess:
			//setPreprocessMode();
			Intent intentPre=new Intent(MainActivity.this,PreProcessActivity.class);
			startActivity(intentPre);
			break;
		case R.id.is_track:
			//setTrackMode();
			Intent intentTra=new Intent(MainActivity.this,TrackingActivity.class);
			startActivity(intentTra);
			break;
		case R.id.pre_opencam:
			preOpenCamera();
			break;
		case R.id.pre_first:
			setFirstFlag();
			break;
		case R.id.pre_second:
			setSecondFlag();
			break;
		case R.id.pre_start:
			setStartPrepTrackFlag();
			break;
		case R.id.pre_store:
			storeMap();
			break;
			
		case R.id.trk_opencam:
			trackOpenCamera();
			break;
		case R.id.trk_lod:
			loadMap();
			break;
		case R.id.trk_start:
			setStartTrackFlag();
			break;
		case R.id.exit:
			SysApplication.getInstance().exit();
			break;
		}
		return super.onOptionsItemSelected(item);
	}

	private void setTrackMode() {
		// TODO Auto-generated method stub
		mode=trackMode;
		showToast("now in track mode");
	}


	private void setPreprocessMode() {
		// TODO Auto-generated method stub
		mode=preprocessMode;
		showToast("now in preprocess mode");
	}
	
	private boolean inPreprocessMode()
	{
		return mode==preprocessMode;
	}
	
	private boolean inTrackMode()
	{
		return mode==trackMode;
	}
	
	private void showToast(String toastMesage)
	{
		Looper.getMainLooper();
		Toast toast = Toast.makeText(this, toastMesage, Toast.LENGTH_LONG);
		
		toast.show();
		Looper.loop();
	}


	private void setStartTrackFlag() {
		// TODO Auto-generated method stub
		if(inPreprocessMode())
		{
			showToast("now in preprocess mode, can't choose this");
			return;
		}
		ARNativeLib.setStartTrackFlagNative();
	}


	private void loadMap() {
		// TODO Auto-generated method stub
		if(inPreprocessMode())
		{
			showToast("now in preprocess mode, can't choose this");
			return;
		}
		ARNativeLib.loadMapNative();
	}


	private void trackOpenCamera() {
		// TODO Auto-generated method stub
		if(inPreprocessMode())
		{
			showToast("now in preprocess mode, can't choose this");
			return;
		}
	}


	private void storeMap() {
		// TODO Auto-generated method stub
		if(inTrackMode())
		{
			showToast("now in track mode, can't choose this");
			return;
		}
		ARNativeLib.storeMapNative();
	}


	private void setStartPrepTrackFlag() {
		// TODO Auto-generated method stub
		if(inTrackMode())
		{
			showToast("now in track mode, can't choose this");
			return;
		}
		ARNativeLib.setStartPrepTrackFlagNative();
		showToast("start preprocess track");

	}


	private void setSecondFlag() {
		// TODO Auto-generated method stub
		if(inTrackMode())
		{
			showToast("now in track mode, can't choose this");
			return;
		}
		ARNativeLib.setSecondFlagNative();
		showToast("get second frame");
	}


	private void setFirstFlag() {
		// TODO Auto-generated method stub
		if(inTrackMode())
		{
			showToast("now in track mode, can't choose this");
			return;
		}
		ARNativeLib.setFirstFlagNative();
		showToast("get first frame");
	}


	private void preOpenCamera() {
		// TODO Auto-generated method stub
		if(inTrackMode())
		{
			showToast("now in track mode, can't choose this");
			return;
		}
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			return rootView;
		}
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		Log.i(TAG, "called camera start");
		mGray = new Mat();
        mRgba = new Mat();
        Log.i(TAG, "called camera end");
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
		//Log.i(TAG, "called frame start");
		mRgba = inputFrame.rgba();
		mGray = inputFrame.gray();
		/*try {
			Thread.sleep(20);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
        //Log.i(TAG, "called frame end");
		int state=ARNativeLib.preprocessFrameNative(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
		Log.e(TAG, ""+state);
		if(state==gottenFirst)
		{
			Log.e(TAG, "gotten first");
			//showToast("first frame gotten");
		}
		else if(state==initFailed)
		{
			Log.e(TAG, "gotten second");
			//showToast("init map failed");
		}
		else if(state==initSuccess)
		{
			//showToast("init map success");
		}
		else if(state==trackFailed)
		{
			//showToast("tracking failed");
		}
		else if(state==triangulateFailed)
		{
			//showToast("triangulation failed");
		}
        //FindFeatures(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
        return mRgba;
        
	}

	/*@Override
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		mGray = new Mat();
        mRgba = new Mat();
	}

	@Override
	public void onCameraViewStopped() {
		// TODO Auto-generated method stub
		mGray.release();
        mRgba.release();
	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		// TODO Auto-generated method stub
		mRgba = inputFrame.rgba();
        Imgproc.cvtColor(mRgba, mGray, Imgproc.COLOR_BGRA2GRAY);
        return mGray;
	}*/
	/*public native void FindFeatures(long matAddrGr, long matAddrRgba);
	
	public native void processFrameNative(long matAddrGr, long matAddrRgba);
	
	public native void setFirstFlagNative();
	
	public native void setSecondFlagNative();
	
	public native void storeMapNative();
	
	public native void setStartPrepTrackFlagNative();
	
	static {
        System.loadLibrary("AugmentedReality");
    }
	
	public native void setStartTrackFlagNative();
	
	public native void loadMapNative();
	
	public native void redirectStdOut();*/

}
