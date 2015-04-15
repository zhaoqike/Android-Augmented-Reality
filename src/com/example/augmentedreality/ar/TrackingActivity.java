package com.example.augmentedreality.ar;

import java.util.ListIterator;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import com.example.augmentedreality.ARActivity;
import com.example.augmentedreality.R;
import com.example.augmentedreality.gljni.GLJNIView;
import com.example.augmentedreality.ar.CameraView;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.hardware.Camera.Size;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;

public class TrackingActivity extends ARActivity implements CvCameraViewListener2,State {

	private static final String TAG = "track activity";
	private Mat mRgba;
	private Mat mGray;
	//private CameraBridgeViewBase mOpenCvCameraView;
	private CameraView mCameraView;
    GLJNIView glSurfaceView;
    private ARCubeRenderer mARRenderer;


	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i(TAG, "OpenCV loaded successfully");
                System.loadLibrary("AugmentedReality");
                mCameraView.enableFpsMeter();
                mCameraView.enableView();
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
		
		FrameLayout layout = new FrameLayout(this);
        layout.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        setContentView(layout);
        
        //mCameraView = new NativeCameraView(this,0);
        mCameraView=new CameraView(this, 0);
        mCameraView.setResolution(640,480);
        mCameraView.setCvCameraViewListener(this);
        mCameraView.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        layout.addView(mCameraView);
        
        glSurfaceView = new GLJNIView(this);
        glSurfaceView.setHandler(new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                
                // œ‘ æfps
                //mTextView.setText("fps:"+msg.what);
            }
        }
        );
        //glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        
        glSurfaceView.setZOrderOnTop(true);
        glSurfaceView.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        
        mARRenderer = new ARCubeRenderer();

        glSurfaceView.setRenderer(mARRenderer);
        
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        layout.addView(glSurfaceView);
	}

	@Override
	public void onPause()
	{
		super.onPause();
		if (mCameraView != null)
			mCameraView.disableView();
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
		mCameraView.disableView();
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.track, menu);

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
		case R.id.trk_lod:
			ARNativeLib.loadMapNative();
			showToast("map load");
			break;
		case R.id.trk_start:
			ARNativeLib.setStartTrackFlagNative();
			break;
		}

		return super.onOptionsItemSelected(item);
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
		int state=ARNativeLib.trackingFrameNative(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
		if(state==triangulateSuccess || state==triangulateFailed)
		{
			Log.e(TAG,""+state);
		}
		if(state==gottenFirst)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("first frame gotten");
	            }
	        });
			//showToast("first frame gotten");
		}
		else if(state==initFailed)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("init map failed");
	            }
	        });
			//showToast("init map failed");
		}
		else if(state==initSuccess)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("init map success");
	            }
	        });
			//showToast("init map success");
		}
		else if(state==trackSuccess)
		{
			glSurfaceView.requestRender();
		}
		else if(state==trackFailed)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("tracking failed");
	            }
	        });
			//showToast("tracking failed");
		}
		else if(state==triangulateFailed)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("triangulation failed");
	            }
	        });
			//showToast("triangulation failed");
		}
		else if(state==triangulateSuccess)
		{
			runOnUiThread(new Runnable() {
	            @Override
	            public void run() {
	            	showToast("triangulation success");
	            }
	        });
			//showToast("triangulation failed");
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
