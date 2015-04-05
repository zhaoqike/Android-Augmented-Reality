package com.example.augmentedreality.ar;

import java.util.List;
import java.util.ListIterator;

import javax.microedition.khronos.opengles.GL10;

import com.example.augmentedreality.ar.ARCubeRenderer;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.NativeCameraView;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import com.example.augmentedreality.ARActivity;
import com.example.augmentedreality.MainActivity;
import com.example.augmentedreality.R;
import com.example.augmentedreality.osg.osgViewer;







import android.app.Activity;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.hardware.Camera.Size;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;

public class PreProcessActivity extends ARActivity implements
		CvCameraViewListener2, State {
	
	private static final String TAG = "preprocess activity";
	private Mat mRgba;
    private Mat mGray;
    //private CameraBridgeViewBase mCameraView;
    private CameraView mCameraView;
    GLSurfaceView glSurfaceView;
    private ARCubeRenderer mARRenderer;
    
    //menu resolution
    private List<Size> mResolutionList;
    private MenuItem[] mEffectMenuItems;
    private SubMenu mColorEffectsMenu;
    private MenuItem[] mResolutionMenuItems;
    private SubMenu mResolutionMenu;
    
    
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
        mCameraView.setCvCameraViewListener(this);
        mCameraView.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        layout.addView(mCameraView);
        
        glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glSurfaceView.getHolder().setFormat(
                PixelFormat.TRANSLUCENT);
        
        glSurfaceView.setZOrderOnTop(true);
        glSurfaceView.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        
        mARRenderer = new ARCubeRenderer();

        glSurfaceView.setRenderer(mARRenderer);
        
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        layout.addView(glSurfaceView);
        

        
        
		
        /*setContentView(R.layout.camera_view_layout);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById ( R.id.camera_view );
        if(mOpenCvCameraView==null)
        {
        	Log.e(TAG, "mopencvcameraview is null");
        }
        mOpenCvCameraView.setVisibility( SurfaceView.VISIBLE );
        mOpenCvCameraView.setCvCameraViewListener(this);
        //int height=mOpenCvCameraView.getMinimumHeight();
        //int width=mOpenCvCameraView.getMinimumWidth();
        //mOpenCvCameraView.setMaxFrameSize(width, height);
        Log.i(TAG, "called onCreate end");
        //Log.i(TAG,"width: "+Integer.toString(width)+"   height: "+Integer.toString(height));
        Intent startServiceIntent = new Intent("com.example.augmentedreality.ar.intentservice");  
        Bundle bundle = new Bundle();  
        bundle.putString("param", "oper1");  
        startServiceIntent.putExtras(bundle);  
        startService(startServiceIntent);  */
        
        //redirectStdOut();
        //SysApplication.getInstance().addActivity(this); 
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
		getMenuInflater().inflate(R.menu.preprocess, menu);
		/*preprocessSubMenu=menu.addSubMenu("preprocess");
		preprocessSubMenu.add(0,PRE_FIRST,0,"first frame");
		preprocessSubMenu.add(0,PRE_SECOND,0,"second frame");
		preprocessSubMenu.add(0,PRE_START,0,"start tracker");
		preprocessSubMenu.add(0,PRE_STORE,0,"store map");
		trackerSubMenu=menu.addSubMenu("tracker");
		trackerSubMenu.add(0,TRK_LOD,0,"load map");
		trackerSubMenu.add(0,TRK_START,0,"start tracker");*/
		
		
		//resolution
		mResolutionMenu = menu.addSubMenu("Resolution");
        mResolutionList = mCameraView.getResolutionList();
        mResolutionMenuItems = new MenuItem[mResolutionList.size()];

        ListIterator<Size> resolutionItr = mResolutionList.listIterator();
        int idx = 0;
        while(resolutionItr.hasNext()) {
            Size element = resolutionItr.next();
            mResolutionMenuItems[idx] = mResolutionMenu.add(2, idx, Menu.NONE,
                    Integer.valueOf(element.width).toString() + "x" + Integer.valueOf(element.height).toString());
            idx++;
        }

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
		case R.id.pre_opencam:
			//preOpenCamera();
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
			
		}
		
		
		//resolution
		if (item.getGroupId() == 2)
        {
            int id = item.getItemId();
            Size resolution = mResolutionList.get(id);
            mCameraView.setResolution(resolution);
            resolution = mCameraView.getResolution();
            String caption = Integer.valueOf(resolution.width).toString() + "x" + Integer.valueOf(resolution.height).toString();
            //set ARCubeRenderer
            //GL10 gl = new gl;
            //mARRenderer.setFrustum(gl, resolution.width, resolution.height);
            Toast.makeText(this, caption, Toast.LENGTH_SHORT).show();
            //glSurfaceView.onPause();
            //mARRenderer.setSize(resolution.width, resolution.height);
            //glSurfaceView.onResume();
        }
		return super.onOptionsItemSelected(item);
	}
	
	
	private void storeMap() {
		// TODO Auto-generated method stub
		ARNativeLib.storeMapNative();
	}


	private void setStartPrepTrackFlag() {
		// TODO Auto-generated method stub
		ARNativeLib.setStartPrepTrackFlagNative();
		showToast("start preprocess track");

	}


	private void setSecondFlag() {
		// TODO Auto-generated method stub
		ARNativeLib.setSecondFlagNative();
		showToast("get second frame");
	}


	private void setFirstFlag() {
		// TODO Auto-generated method stub
		ARNativeLib.setFirstFlagNative();
		showToast("get first frame");
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
		//ARNativeLib.sendFrameToNative(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
		//int state=0;
		int state=ARNativeLib.preprocessFrameNative(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
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
