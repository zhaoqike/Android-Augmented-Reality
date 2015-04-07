/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * author: mnorst@foxmail.com
 */

package com.example.augmentedreality.gljni;

import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.example.augmentedreality.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.os.Handler;
import android.util.Log;

/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform
 * OpenGL ES 1.x rendering into a GL Surface.
 */
public class GLJNIView extends GLSurfaceView {

	private static final String LOG_TAG = GLJNIView.class.getSimpleName();

	private Renderer renderer;

	public GLJNIView(Context context) {
		super(context);

		// setEGLConfigChooser���fps����Ӱ��
		setEGLConfigChooser(8, 8, 8, 8, 16, 0);

		renderer = new Renderer(context);
		//setRenderer(renderer);
	}
	
	public void setHandler( Handler handler){
		renderer.setHandler(handler);
	}

	private static class Renderer implements GLSurfaceView.Renderer {
		//��������ӳ��İ󶨣����Ѱ󶨺��ID���ݸ�C++���룬�������
		private int[] mTexture = new int[2];
		//���ڼ���Bitmap��context
		private Context mContext;
		
		// ͳ��fps
		private Handler mHandler;
		private long mStartMili;			
		private long mEndMili;
		private int mFps = 0;
		
		public Renderer(Context ctx) {
			mContext = ctx;
			mStartMili =System.currentTimeMillis();
		}
		
		public void setHandler( Handler handler){
			mHandler = handler;
		}

		public void onDrawFrame(GL10 gl) {
			
			GLJNILib.step();
			
			// ��һ���ӻ��Ƶ�֡����ͳ��fps
			mEndMili = System.currentTimeMillis();
			if( mEndMili - mStartMili > 1000 ){
				mHandler.sendEmptyMessageDelayed(mFps, 100);
				mStartMili = mEndMili;
				mFps = 0;
			}
			mFps++;
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			GLJNILib.resize(width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			//������Bitmap����
			genTexture(gl, mContext);
			//���ñ���setTexture������������󶨵�ID���ݸ�C++���룬�Թ������
			GLJNILib.setTexture(mTexture);
			GLJNILib.init();
		}
		
		/**
		 * ����Bitmap�ķ�����
		 * ������res�м���Bitmap��Դ
		 * */
		private Bitmap loadBitmap(Context context, int resourceId) {
			InputStream is = context.getResources().openRawResource(resourceId);
			Bitmap bitmap = null;
			try {

				// ����BitmapFactory����Bitmap
				bitmap = BitmapFactory.decodeStream(is);
			} finally {
				try {

					// �ر���
					is.close();
					is = null;
				} catch (IOException e) {
					e.printStackTrace();
				}

			}
			return bitmap;

		}
		
		/**
		 * ��Bitmap����
		 * */

		private void genTexture(GL10 gl, Context context) {
			//��������
			gl.glGenTextures(2, mTexture, 0);
			//����Bitmap
			Bitmap bitmap = loadBitmap(context, R.drawable.logo);
			if (bitmap != null) {
				//���bitmap���سɹ��������ɴ�bitmap������ӳ��
				gl.glBindTexture(GL10.GL_TEXTURE_2D, mTexture[0]);
				//��������ӳ�������
				gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,
						GL10.GL_NEAREST);
				gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,
						GL10.GL_NEAREST);
				//��������ӳ��
				GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
				//�ͷ�bitmap��Դ
				bitmap.recycle();
			}

		}
	}

}
