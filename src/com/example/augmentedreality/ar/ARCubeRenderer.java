package com.example.augmentedreality.ar;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;

import org.opencv.calib3d.Calib3d;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;

import com.example.augmentedreality.opengl.CameraCalibration;
import com.example.augmentedreality.opengl.ReadText;
import com.example.augmentedreality.opengl.geometry.GLTransformation;
import com.example.augmentedreality.opengl.geometry.Matrix33;
import com.example.augmentedreality.opengl.geometry.Matrix44;
import com.example.augmentedreality.opengl.geometry.Vector3;

import android.R.string;
import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLUtils;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;

public class ARCubeRenderer implements Renderer{
	private static final String TAG = "ARCubeRender";
	boolean m_isTextureInitialized=false;
	int[] m_backgroundTextureId=new int[1];
	int backgroundTexture;
	
	int screenWidth=640;
	int screenHeight=480;
	
	static float PI=3.141592653f;
	
	
	//int cameraWidth=0;
	//int cameraHeight=0;

	boolean hasCenter=false;
	//Vector3 center;
	float centerx;
	float centery;
	float centerz;
	ReadText reader=new ReadText();

	CameraCalibration calibration;
	GLTransformation patternPose=new GLTransformation();
	// ������Ķ������꣨һ����36�����㣬���12�������Σ�
	private float[] cubeVertices = { -0.6f, -0.6f, -0.6f, -0.6f, 0.6f,
			-0.6f, 0.6f, 0.6f, -0.6f, 0.6f, 0.6f, -0.6f, 0.6f, -0.6f, -0.6f,
			-0.6f, -0.6f, -0.6f, -0.6f, -0.6f, 0.6f, 0.6f, -0.6f, 0.6f, 0.6f,
			0.6f, 0.6f, 0.6f, 0.6f, 0.6f, -0.6f, 0.6f, 0.6f, -0.6f, -0.6f,
			0.6f, -0.6f, -0.6f, -0.6f, 0.6f, -0.6f, -0.6f, 0.6f, -0.6f, 0.6f,
			0.6f, -0.6f, 0.6f, -0.6f, -0.6f, 0.6f, -0.6f, -0.6f, -0.6f, 0.6f,
			-0.6f, -0.6f, 0.6f, 0.6f, -0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f,
			0.6f, 0.6f, -0.6f, 0.6f, 0.6f, -0.6f, -0.6f, 0.6f, 0.6f, -0.6f,
			-0.6f, 0.6f, -0.6f, -0.6f, 0.6f, 0.6f, -0.6f, 0.6f, 0.6f, 0.6f,
			0.6f, 0.6f, 0.6f, 0.6f, -0.6f, -0.6f, 0.6f, -0.6f, -0.6f, -0.6f,
			-0.6f, -0.6f, -0.6f, 0.6f, -0.6f, -0.6f, 0.6f, -0.6f, 0.6f, 0.6f,
			-0.6f, 0.6f, -0.6f, };
	// ��������������Ҫ��6���棨һ����12������������Ķ��㣩
	private byte[] cubeFacets = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, };
	// ����������ͼ����������
	private float[] cubeTextures = { 1.0000f, 1.0000f, 1.0000f, 0.0000f,
			0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 1.0000f, 1.0000f,
			1.0000f, 0.0000f, 1.0000f, 1.0000f, 1.0000f, 1.0000f, 0.0000f,
			1.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 1.0000f, 0.0000f,
			1.0000f, 1.0000f, 1.0000f, 1.0000f, 0.0000f, 1.0000f, 0.0000f,
			0.0000f, 0.0000f, 0.0000f, 1.0000f, 0.0000f, 1.0000f, 1.0000f,
			1.0000f, 1.0000f, 0.0000f, 1.0000f, 0.0000f, 0.0000f, 0.0000f,
			0.0000f, 1.0000f, 0.0000f, 1.0000f, 1.0000f, 1.0000f, 1.0000f,
			0.0000f, 1.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 1.0000f,
			0.0000f, 1.0000f, 1.0000f, 1.0000f, 1.0000f, 0.0000f, 1.0000f,
			0.0000f, 0.0000f, 0.0000f, 0.0000f, 1.0000f };

	private Context context;
	private FloatBuffer cubeVerticesBuffer;
	private ByteBuffer cubeFacetsBuffer;
	private FloatBuffer cubeTexturesBuffer;
	// ���屾������ʹ�õ�����
	private int texture;

	public ARCubeRenderer()
	{
		// ��������Ķ���λ�����������װ��FloatBuffer;
		cubeVerticesBuffer = floatBufferUtil(cubeVertices);
		// ���������6���棨12�������Σ��������װ��ByteBuffer
		cubeFacetsBuffer = ByteBuffer.wrap(cubeFacets);
		// ���������������ͼ���������ݰ�װ��FloatBuffer
		cubeTexturesBuffer = floatBufferUtil(cubeTextures);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		Log.e(TAG,"onSurfaceCreated");
		// �رտ�����
		gl.glDisable(GL10.GL_DITHER);
		// ����ϵͳ��͸�ӽ�������
		gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_FASTEST);
		gl.glClearColor(0, 0, 0, 0);
		// ������Ӱƽ��ģʽ
		gl.glShadeModel(GL10.GL_SMOOTH);
		// ������Ȳ���
		gl.glEnable(GL10.GL_DEPTH_TEST);
		// ������Ȳ��Ե�����
		gl.glDepthFunc(GL10.GL_LEQUAL);
		// ����2D������ͼ
		gl.glEnable(GL10.GL_TEXTURE_2D);
		// װ������
		//loadTexture(gl);
	}
	
	
	static void _gluPerspective(GL10 gl, float fovy, float aspect, float zNear, float zFar)
	{
		float top = zNear * ((float) Math.tan(fovy * PI / 360.0));
		float bottom = -top;
		float left = bottom * aspect;
		float right = top * aspect;
		gl.glFrustumf(left, right, bottom, top, zNear, zFar);
	}

	void resize(GL10 gl, int width, int height)
	{
		// ��ֹ�����
		if (height==0)								
		{
			height=1;
		}

		int xstart=(width-screenWidth)/2;
		int ystart=(height-screenHeight)/2;
		// ���õ�ǰ���ӿ�
		//glViewport(0, 0, width, height);
		//glViewport(xstart, ystart, 640, 480);


		//scale window to fit parent window
		double scalew=(double)width/(double)screenWidth;
		double scaleh=(double)height/(double)screenHeight;
		int dispw=width;
		int disph=height;
		if(scalew>scaleh)
		{
			dispw=(int)(scaleh*(double)screenWidth);
		}
		else
		{
			disph=(int)(scalew*(double)screenHeight);
		}
		int startx=(width-dispw)/2;
		int starty=(height-disph)/2;
		gl.glViewport(startx, starty, dispw, disph);


		// ѡ��ͶӰ����	
		gl.glMatrixMode(GL10.GL_PROJECTION);	
		// ����ͶӰ����	
		gl.glLoadIdentity();							

		// �����ӿڵĴ�С
		//_gluPerspective(45.0f,(float)width/(float)height,0.1f,100.0f);
		_gluPerspective(gl, 45.0f,(float)screenWidth/(float)screenHeight,0.1f,100.0f);

		// ѡ��ģ�͹۲����
		gl.glMatrixMode(GL10.GL_MODELVIEW);	

		// ����ģ�͹۲����
		gl.glLoadIdentity();							
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		Log.e(TAG,"onSurfaceChanged");
		/*int viewWidth=width;
		int viewHeight=height;
		float viewRatio=(float)viewWidth/(float)viewHeight;
		
		
		if(cameraWidth==0 && cameraHeight==0)
		{
			cameraWidth=width;
			cameraHeight=height;
		}
		float w=cameraWidth;
		float h=cameraHeight;
		
		float cameraRatio=w/h;
		if(viewRatio>cameraRatio)
		{
			viewWidth=(int)((float)viewHeight*cameraRatio);
		}
		else {
			viewHeight=(int)((float)viewWidth/cameraRatio);
		}
		Log.e(TAG,""+viewRatio+"  "+cameraRatio);
		Log.e(TAG, ""+viewWidth+"  "+viewHeight);
		Log.e(TAG,""+width+"  "+height);
		int startx=(width-cameraWidth)/2;
		int starty=(height-cameraHeight)/2;
		Log.e(TAG,""+startx+"  "+starty);*/
		// ����3D�Ӵ��Ĵ�С��λ��
		/*gl.glViewport(0, 0, width, height);
		// ����ǰ����ģʽ��ΪͶӰ����
		gl.glMatrixMode(GL10.GL_PROJECTION);
		// ��ʼ����λ����
		gl.glLoadIdentity();
		// ����͸���Ӵ��Ŀ�ȡ��߶ȱ�
		//float ratio = (float) width / height;
		// ���ô˷�������͸���Ӵ��Ŀռ��С��
		//set frustum
		//w=viewWidth;
		//h=viewHeight;
		float w=width;
		float h=height;
		
		float znear = 1.0f;
		float zfar = 100.0f;
		float u0 = w / 2;
		float v0 = h / 2;
		float maxwh = Math.max(w, h);
		float fx = maxwh;
		float fy = maxwh;
		float top = v0*znear / fy;
		float bottom = (v0 - h)*znear / fy;
		float left = -u0*znear / fx;
		float right = (w - u0)*znear / fx;
		gl.glFrustumf(left, right, bottom, top, znear, zfar);*/
		resize(gl, width, height);
	}

	/*public void setSize(int camWidth,int camHeight)
	{
		cameraWidth=camWidth;
		cameraHeight=camHeight;
	}*/

	/*void drawCameraFrame(GL10 gl)                                  // Render background
	{
		// Initialize texture for background image

		if (!m_isTextureInitialized)
		{
			gl.glGenTextures(1, m_backgroundTextureId, 0);
			backgroundTexture=m_backgroundTextureId[0];
			gl.glBindTexture(GL10.GL_TEXTURE_2D, backgroundTexture);

			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);

			m_isTextureInitialized = true;
		}

		Bitmap bitmap = null;
		//bitmap = BitmapFactory.decodeResource(context.getResources(),R.drawable.a80);
		// ����λͼ��������
		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);


		/*m_backgroundImage = imageList[pindex];
		Size size = m_backgroundImage.size();
		if (size.width != 640 || size.height != 480)
		{
			//cout << "background image size error" << endl;
			return;
		}
		int w = m_backgroundImage.cols;
		int h = m_backgroundImage.rows;

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

		// Upload new texture data:
		if (m_backgroundImage.channels() == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, m_backgroundImage.data);
		else if (m_backgroundImage.channels() == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
		else if (m_backgroundImage.channels() == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_backgroundImage.data);

		//float bgTextureVertices[] = { 0, 0, (float)w, 0, 0, (float)h, (float)w, (float)h };
		//float bgTextureCoords[] = { 1, 0, 1, 1, 0, 0, 0, 1 };
		//float proj[] = { 0, -2.f / w, 0, 0, -2.f / h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };
		FloatBuffer projBuffer=floatBufferUtil(proj);
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadMatrixf(projBuffer);

		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();

		gl.glEnable(GL10.GL_TEXTURE_2D);
		//gl.glPixelStoref(GL10.GL_PACK_ALIGNMENT, 1);
		gl.glBindTexture(GL10.GL_TEXTURE_2D, backgroundTexture);

		// Update attribute values.
		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

		FloatBuffer vertexBuffer=floatBufferUtil(bgTextureVertices);
		FloatBuffer coordsBuffer=floatBufferUtil(bgTextureCoords);
		gl.glVertexPointer(2, GL10.GL_FLOAT, 0, vertexBuffer);
		gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, coordsBuffer);

		gl.glColor4f(1, 1, 1, 1);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, 4);

		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
		gl.glDisable(GL10.GL_TEXTURE_2D);
	}*/
	public void drawCameraFrame(GL10 gl){

		gl.glDisable(gl.GL_DEPTH_TEST);

		if (!m_isTextureInitialized)
		{
			gl.glGenTextures(1, m_backgroundTextureId, 0);
			backgroundTexture=m_backgroundTextureId[0];
			gl.glBindTexture(GL10.GL_TEXTURE_2D, backgroundTexture);

			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);

			m_isTextureInitialized = true;
		}

		Bitmap bitmap = null;
		/*if(option==1)
		{
		if(index==0)
		{
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.a80);
		}
		if(index==1)
		{
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.a81);
		}
		if(index==2)
		{
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.a82);
		}
		if(index==3)
		{
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.a83);
		}
		if(index==4)
		{
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.a84);
		}
		}
		else if(option==2)
		{
			if(index==0)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a90);
			}
			if(index==1)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a91);
			}
			if(index==2)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a92);
			}
			if(index==3)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a93);
			}
			if(index==4)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a94);
			}
			if(index==5)
			{
				bitmap = BitmapFactory.decodeResource(context.getResources(),
						R.drawable.a95);
			}
		}*/

		// ����λͼ��������
		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);


		// �����Ļ�������Ȼ���
		/*gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);


		float proj[] = { 0, -2.f / w, 0, 0, -2.f / h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };
		FloatBuffer projBuffer=floatBufferUtil(proj);
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadMatrixf(projBuffer);
		// ���õ�ǰ����ģʽΪģ����ͼ��
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		// �ѻ�ͼ����������Ļ2����λ
		gl.glTranslatef(0f, 0.0f, -2.0f);*/


		// �����Ļ�������Ȼ���
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
		// ���ö�����������
		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		// ������ͼ������������
		gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);  //��


		float proj[] = { 0, -2.f / 640.0f, 0, 0, -2.f / 480.0f, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };
		FloatBuffer projBuffer=floatBufferUtil(proj);
		gl.glMatrixMode(GL10.GL_PROJECTION);
		//gl.glLoadMatrixf(projBuffer);
		// ���õ�ǰ����ģʽΪģ����ͼ��
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		// �ѻ�ͼ����������Ļ2����λ
		gl.glTranslatef(0f, 0.0f, -2.0f);

		float bgTextureCoords[] = {  1, 1, 0, 1, 0, 0, 1, 0};
		float bgTextureVertices[] = { 1, -1,-1, -1,-1, 1, 1, 1  };
		FloatBuffer vertexBuffer=floatBufferUtil(bgTextureVertices);
		FloatBuffer coordsBuffer=floatBufferUtil(bgTextureCoords);

		gl.glColor4f(1, 1, 1, 1);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
		gl.glEnable(GL10.GL_TEXTURE_2D);
		//gl.glPixelStoref(GL10.GL_PACK_ALIGNMENT, 1);
		gl.glBindTexture(GL10.GL_TEXTURE_2D, backgroundTexture);
		gl.glVertexPointer(2, GL10.GL_FLOAT, 0, vertexBuffer);
		gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, coordsBuffer);

		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN, 0, 4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
		gl.glDisable(GL10.GL_TEXTURE_2D);



		gl.glEnable(gl.GL_DEPTH_TEST);
	}

	public void drawCoordinateAxis(GL10 gl)
	{
		float lineX[] = { 0, 0, 0, 1, 0, 0 };
		float lineY[] = { 0, 0, 0, 0, 1, 0 };
		float lineZ[] = { 0, 0, 0, 0, 0, 1 };

		int lineXcolor[]={65535,0,0,0,65535,0,0,0};
		int lineYcolor[]={0,65535,0,0,0,65535,0,0};
		int lineZcollr[]={0,0,65535,0,0,0,65535,0};


		FloatBuffer lxBuffer=floatBufferUtil(lineX);
		FloatBuffer lyBuffer=floatBufferUtil(lineY);
		FloatBuffer lzBuffer=floatBufferUtil(lineZ);


		IntBuffer lxcolorBuffer=intBufferUtil(lineXcolor);
		IntBuffer lycolorBuffer=intBufferUtil(lineYcolor);
		IntBuffer lzcolorBuffer=intBufferUtil(lineZcollr);

		gl.glLineWidth(2);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glEnableClientState(GL10.GL_COLOR_ARRAY);

		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, lxBuffer);
		gl.glColorPointer(4, GL10.GL_FIXED, 0, lxcolorBuffer);
		gl.glDrawArrays(GL10.GL_LINES,0,2);
		//gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		


		//gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, lyBuffer);
		gl.glColorPointer(4, GL10.GL_FIXED, 0, lycolorBuffer);
		gl.glDrawArrays(GL10.GL_LINES,0,2);
		//gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);

		//gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, lzBuffer);
		gl.glColorPointer(4, GL10.GL_FIXED, 0, lzcolorBuffer);
		gl.glDrawArrays(GL10.GL_LINES,0,2);

		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glDisableClientState(GL10.GL_COLOR_ARRAY);

		/*glBegin(GL_LINES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(lineX);
		glVertex3fv(lineX + 3);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3fv(lineY);
		glVertex3fv(lineY + 3);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3fv(lineZ);
		glVertex3fv(lineZ + 3);

		glEnd();*/
	}


	public void drawCubeModel(GL10 gl)
	{
		float LightAmbient[] = { 0.25f, 0.25f, 0.25f, 1.0f };    // Ambient Light Values
		float LightDiffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };    // Diffuse Light Values
		float LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };    // Light Position

		FloatBuffer laBuffer=floatBufferUtil(LightAmbient);
		FloatBuffer ldBuffer=floatBufferUtil(LightDiffuse);
		FloatBuffer lpBuffer=floatBufferUtil(LightPosition);

		//glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);

		gl.glColor4f(0.2f, 0.35f, 0.3f, 0.75f);         // Full Brightness, 50% Alpha ( NEW )
		gl.glBlendFunc(GL10.GL_ONE, GL10.GL_ONE_MINUS_SRC_ALPHA);       // Blending Function For Translucency Based On Source Alpha 
		gl.glEnable(GL10.GL_BLEND);

		gl.glShadeModel(GL10.GL_SMOOTH);

		gl.glEnable(GL10.GL_LIGHTING);
		gl.glDisable(GL10.GL_LIGHT0);
		gl.glEnable(GL10.GL_LIGHT1);
		gl.glLightfv(GL10.GL_LIGHT1, GL10.GL_AMBIENT, laBuffer);
		gl.glLightfv(GL10.GL_LIGHT1, GL10.GL_DIFFUSE, ldBuffer);
		gl.glLightfv(GL10.GL_LIGHT1, GL10.GL_POSITION, lpBuffer);
		gl.glEnable(GL10.GL_COLOR_MATERIAL);

		gl.glScalef(0.15f, 0.15f, 0.15f);
		gl.glTranslatef(0, 0, 1);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glBegin(GL_QUADS);
		// Front Face
		/*glNormal3f(0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 1 (Front)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 2 (Front)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 4 (Front)*/

		//gles
		float q3front[] = {
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f
		};

		FloatBuffer frontBuffer=floatBufferUtil(q3front);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, frontBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);

		// Back Face
		/*glNormal3f(0.0f, 0.0f, -1.0f);    // Normal Pointing Away From Viewer
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 2 (Back)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 3 (Back)
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 4 (Back)*/

		//gles
		float q3back[] = {
				-1.0f, -1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, -1.0f, -1.0f
		};

		FloatBuffer backBuffer=floatBufferUtil(q3back);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, backBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);


		// Top Face
		/*glNormal3f(0.0f, 1.0f, 0.0f);    // Normal Pointing Up
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 1 (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 2 (Top)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Top)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 4 (Top)*/

		//gles
		float q3top[] = {
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f
		};

		FloatBuffer topBuffer=floatBufferUtil(q3top);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, topBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);



		// Bottom Face
		/*glNormal3f(0.0f, -1.0f, 0.0f);    // Normal Pointing Down
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 3 (Bottom)
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 4 (Bottom)*/

		//gles
		float q3bottom[] = {
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f
		};

		FloatBuffer bottomBuffer=floatBufferUtil(q3bottom);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, bottomBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);


		// Right face
		/*glNormal3f(1.0f, 0.0f, 0.0f);    // Normal Pointing Right
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 1 (Right)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 2 (Right)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Right)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 4 (Right)*/

		//gles
		float q3right[] = {
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 1.0f
		};

		FloatBuffer rightBuffer=floatBufferUtil(q3right);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, rightBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);


		// Left Face
		/*glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 2 (Left)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 3 (Left)
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 4 (Left)*/


		//gles
		float q3left[] = {
				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f
		};

		FloatBuffer leftBuffer=floatBufferUtil(q3left);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, leftBuffer);
		gl.glDrawArrays(GL10.GL_TRIANGLE_FAN,0,4);
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);


		//glEnd();

		/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.2f, 0.65f, 0.3f, 0.35f); // Full Brightness, 50% Alpha ( NEW )
		glBegin(GL_QUADS);
		// Front Face
		glNormal3f(0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 1 (Front)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 2 (Front)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 4 (Front)
		// Back Face
		glNormal3f(0.0f, 0.0f, -1.0f);    // Normal Pointing Away From Viewer
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 2 (Back)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 3 (Back)
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 4 (Back)
		// Top Face
		glNormal3f(0.0f, 1.0f, 0.0f);    // Normal Pointing Up
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 1 (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 2 (Top)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Top)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 4 (Top)
		// Bottom Face
		glNormal3f(0.0f, -1.0f, 0.0f);    // Normal Pointing Down
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 3 (Bottom)
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 4 (Bottom)
		// Right face
		glNormal3f(1.0f, 0.0f, 0.0f);    // Normal Pointing Right
		glVertex3f(1.0f, -1.0f, -1.0f);  // Point 1 (Right)
		glVertex3f(1.0f, 1.0f, -1.0f);  // Point 2 (Right)
		glVertex3f(1.0f, 1.0f, 1.0f);  // Point 3 (Right)
		glVertex3f(1.0f, -1.0f, 1.0f);  // Point 4 (Right)
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
		glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
		glVertex3f(-1.0f, -1.0f, 1.0f);  // Point 2 (Left)
		glVertex3f(-1.0f, 1.0f, 1.0f);  // Point 3 (Left)
		glVertex3f(-1.0f, 1.0f, -1.0f);  // Point 4 (Left)
		glEnd();

		glPopAttrib();*/
	}


	void drawAugmentedScene(GL10 gl)
	{
		// Init augmentation projection
		/*Matrix44 projectionMatrix = new Matrix44();
		int w = 640;
		int h = 480;
		buildProjectionMatrix(calibration, w, h, projectionMatrix);

		gl.glMatrixMode(gl.GL_PROJECTION);
		float[] data=new float[16];
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				data[i*4+j]=projectionMatrix.mat[i][j];
			}
		}
		FloatBuffer dataBuffer=floatBufferUtil(data);
		gl.glLoadMatrixf(dataBuffer);*/
		gl.glClearColor(0, 0, 0, 0);
		gl.glMatrixMode(gl.GL_MODELVIEW);
		gl.glLoadIdentity();

		if (true)
		{
			// Set the pattern transformation
			//patternPose = getGLPose();
			//Matrix44 glMatrix = patternPose.getMat44();
			float[] glPose=getGLPose();
			Log.e(TAG, "this is before");
			
			if(glPose==null){
				return;
			}
			Log.e(TAG, "this is after");
			/*float[] mdata=new float[16];
			for(int i=0;i<4;i++)
			{
				for(int j=0;j<4;j++)
				{
					mdata[i*4+j]=glMatrix.mat[i][j];
				}
			}
			String indexstring="";
			//indexstring+=index;
			indexstring+="\n";
			String mdatastring="";
			for(int i=0;i<16;i++)
			{
				mdatastring+=mdata[i]+"  ";
			}
			mdatastring+="\n";

			String glPosestringString="";
			for(int i=0;i<16;i++)
			{
				glPosestringString+=patternPose.GLPose[i]+"  ";
			} 
			glPosestringString+="\n";

			appendMethod("sdcard/glpose.txt", indexstring+mdatastring+glPosestringString);

			Log.e(TAG, mdatastring);
			FloatBuffer mdataBuffer=floatBufferUtil(mdata);*/
			FloatBuffer glposeBuffer=floatBufferUtil(glPose);
			gl.glLoadMatrixf(glposeBuffer);
			gl.glTranslatef(centerx, -centery, -centerz);
			//gl.glTranslatef(0, 0, -centerz);
			gl.glScalef(15, 15, 15);

			// Render model
			drawCoordinateAxis(gl);
			//drawCubeModel(gl);
		}
	}

	private boolean getCenter(){
		if(hasCenter==true)
		{
			//return true;
		}
		float[] c=new float[3];
		ARNativeLib.getCenterNative(c);
		centerx=c[0];
		centery=c[1];
		centerz=c[2];
		hasCenter=true;
		return true;
	}
	private float[] getGLPose() {
		// TODO Auto-generated method stub
		getCenter();
		float[] val=new float[12];
		boolean state=ARNativeLib.getGLPoseNative(val);
		if(state==false)
		{
			//Log.e(TAG, "state is false");
			return null;
		}

		//to transformation
		Matrix33 r=new Matrix33();
		r.mat[0][0] = val[0];
		r.mat[0][1] = val[1];
		r.mat[0][2] = val[2];

		r.mat[1][0] = val[4];
		r.mat[1][1] = val[5];
		r.mat[1][2] = val[6];

		r.mat[2][0] = val[8];
		r.mat[2][1] = val[9];
		r.mat[2][2] = val[10];

		Mat rotMat=new Mat(3,3,CvType.CV_64FC1);
		rotMat.put(0, 0, (double)val[0]);
		rotMat.put(0, 1, (double)val[1]);
		rotMat.put(0, 2, (double)val[2]);

		rotMat.put(1, 0, (double)val[4]);
		rotMat.put(1, 1, (double)val[5]);
		rotMat.put(1, 2, (double)val[6]);

		rotMat.put(2, 0, (double)val[8]);
		rotMat.put(2, 1, (double)val[9]);
		rotMat.put(2, 2, (double)val[10]);

		MatOfDouble mRVec=new MatOfDouble();
		Calib3d.Rodrigues(rotMat, mRVec);

		//MatOfDouble rvec=new MatOfDouble(mRVec);
		double[] rVecArray=mRVec.toArray();
		rVecArray[1] *= -1.0;
		rVecArray[2] *= -1.0;
		mRVec.fromArray(rVecArray);

		MatOfDouble mRotation=new MatOfDouble();
		Calib3d.Rodrigues(mRVec, mRotation);


		Vector3 t=new Vector3();
		t.data[0] = val[3];
		t.data[1] = val[7];
		t.data[2] = val[11];

		Mat tMat=new Mat(1,3,CvType.CV_64FC1);
		tMat.put(0, 0, (double)val[3]);
		tMat.put(0, 1, (double)val[7]);
		tMat.put(0, 2, (double)val[11]);

		MatOfDouble mTVec=new MatOfDouble(tMat);

		double[] tVecArray = mTVec.toArray();

		float[] mGLPose=new float[16];
		mGLPose[0]  =  (float)mRotation.get(0, 0)[0];
		mGLPose[1]  =  (float)mRotation.get(1, 0)[0];
		mGLPose[2]  =  (float)mRotation.get(2, 0)[0];
		mGLPose[3]  =  0f;
		mGLPose[4]  =  (float)mRotation.get(0, 1)[0];
		mGLPose[5]  =  (float)mRotation.get(1, 1)[0];
		mGLPose[6]  =  (float)mRotation.get(2, 1)[0];
		mGLPose[7]  =  0f;
		mGLPose[8]  =  (float)mRotation.get(0, 2)[0];
		mGLPose[9]  =  (float)mRotation.get(1, 2)[0];
		mGLPose[10] =  (float)mRotation.get(2, 2)[0];
		mGLPose[11] =  0f;
		mGLPose[12] =  (float)tVecArray[0];
		mGLPose[13] = -(float)tVecArray[1];
		mGLPose[14] = -(float)tVecArray[2];
		mGLPose[15] =  1f;
		String cString="";
		cString+=centerx+"  ";
		cString+=centery+"  ";
		cString+=centerz+"  ";
		//Log.e(TAG, "center: "+cString);
		/*String glString="";
		for(int i=0;i<mGLPose.length;i++)
		{
			glString+=mGLPose[i]+" ";
		}*/
		//Log.e(TAG, "mGLPose: "+glString);
		return mGLPose;
	}

	public void appendMethod(String fileName, String content) {
		try {
			//��һ��д�ļ��������캯���еĵڶ�������true��ʾ��׷����ʽд�ļ�
			FileWriter writer = new FileWriter(fileName, true);
			writer.write(content);
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/*public Matrix44 transformAxis(Matrix44 m)
	{

	}*/

	void buildProjectionMatrix(CameraCalibration calibration, int screen_width, int screen_height, Matrix44 projectionMatrix)
	{
		Log.e(TAG, "begin build calibration");
		if(calibration==null){
			Log.e(TAG, "calibration null");
		}
		float nearPlane = 0.01f;  // Near clipping distance
		float farPlane = 100.0f;  // Far clipping distance

		// Camera parameters
		float f_x = calibration.fx(); // Focal length in x axis
		Log.e(TAG, ""+f_x);
		float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
		float c_x = calibration.cx(); // Camera primary point x
		float c_y = calibration.cy(); // Camera primary point y

		projectionMatrix.mat[0][0] = -2.0f * f_x / screen_width;
		projectionMatrix.mat[0][1] = 0.0f;
		projectionMatrix.mat[0][2] = 0.0f;
		projectionMatrix.mat[0][3] = 0.0f;

		projectionMatrix.mat[1][0] = 0.0f;
		projectionMatrix.mat[1][1] = 2.0f * f_y / screen_height;
		projectionMatrix.mat[1][2] = 0.0f;
		projectionMatrix.mat[1][3] = 0.0f;

		projectionMatrix.mat[2][0] = 2.0f * c_x / screen_width - 1.0f;
		projectionMatrix.mat[2][1] = 2.0f * c_y / screen_height - 1.0f;
		projectionMatrix.mat[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		projectionMatrix.mat[2][3] = -1.0f;

		projectionMatrix.mat[3][0] = 0.0f;
		projectionMatrix.mat[3][1] = 0.0f;
		projectionMatrix.mat[3][2] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
		projectionMatrix.mat[3][3] = 0.0f;
	}


	public void onDrawFrame(GL10 gl)
	{
		//drawCameraFrame(gl);

		/*// �����Ļ�������Ȼ���
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
		// ���ö�����������
		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		// ������ͼ������������
		gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);  //��
		// ���õ�ǰ����ģʽΪģ����ͼ��
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		// �ѻ�ͼ����������Ļ2����λ
		gl.glTranslatef(0f, 0.0f, -2.0f);*/
		//gl.glDisable(gl.GL_DEPTH_TEST);
		//drawCameraFrame(gl);
		//gl.glEnable(gl.GL_DEPTH_TEST);
		//gl.glFinish();
		// ��תͼ��
		//gl.glRotatef(angley, 0, 1, 0);
		//gl.glRotatef(anglex, 1, 0, 0);

		//drawCoordinateAxis(gl);
		//drawCubeModel(gl);
		drawAugmentedScene(gl);
		/*Matrix44 projectionMatrix = new Matrix44();
		int w = 640;
		int h = 480;
		//buildProjectionMatrix(calibration, w, h, projectionMatrix);

		gl.glMatrixMode(gl.GL_PROJECTION);
		float[] data=new float[16];
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				data[i*4+j]=projectionMatrix.mat[i][j];
			}
		}
		FloatBuffer dataBuffer=floatBufferUtil(data);
		//gl.glLoadMatrixf(dataBuffer);

		gl.glMatrixMode(gl.GL_MODELVIEW);
		gl.glLoadIdentity();*/

		/*gl.glMatrixMode(gl.GL_MODELVIEW);
		gl.glLoadIdentity();
		gl.glTranslatef(0f, 0.0f, -3.0f);
		drawCoordinateAxis(gl);*/
		// ���ö����λ������
		//gl.glVertexPointer(3, GL10.GL_FLOAT, 0, cubeVerticesBuffer);
		// ������ͼ����������
		//gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, cubeTexturesBuffer);//��

		// ִ��������ͼ
		//gl.glBindTexture(GL10.GL_TEXTURE_2D, texture);  //��
		// ��cubeFacetsBufferָ���������������

		//gl.glDrawElements(GL10.GL_TRIANGLES, cubeFacetsBuffer.remaining(), GL10.GL_UNSIGNED_BYTE, cubeFacetsBuffer);
		// ���ƽ���
		//gl.glFinish();
		// ���ö��㡢������������
		//gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		//gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
		// �����Ƕ�ֵ�Ա�ÿ���Բ�ͬ�ǶȻ���
	}

	/*private void loadTexture(GL10 gl)
	{
		Bitmap bitmap = null;
		try
		{
			// ����λͼ
			bitmap = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.sand);
			int[] textures = new int[1];
			// ָ������N��������һ������ָ������1��������
			// textures���齫����洢��������Ĵ��š�
			gl.glGenTextures(1, textures, 0);
			// ��ȡtextures���������еĵ�һ������
			texture = textures[0];
			// ֪ͨOpenGL��texture����󶨵�GL10.GL_TEXTURE_2DĿ����
			gl.glBindTexture(GL10.GL_TEXTURE_2D, texture);
			// ����������С�������ӵ��Զʱ����С��ʱ����˲���ʽ
			gl.glTexParameterf(GL10.GL_TEXTURE_2D,
					GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
			// ���������Ŵ󣨾����ӵ�ܽ�ʱ��������ʱ����˲���ʽ
			gl.glTexParameterf(GL10.GL_TEXTURE_2D,
					GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
			// �����ں��������϶���ƽ������
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,
					GL10.GL_REPEAT);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,
					GL10.GL_REPEAT);
			// ����λͼ��������
			GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
		}
		finally
		{
			// ��������֮�󣬻���λͼ
			if (bitmap != null)
				bitmap.recycle();
		}
	}*/

	// ����һ�����߷�������int[]����ת��ΪOpenGL ES�����IntBuffer
	private IntBuffer intBufferUtil(int[] arr)
	{
		IntBuffer mBuffer;
		// ��ʼ��ByteBuffer������Ϊarr����ĳ���*4����Ϊһ��intռ4���ֽ�
		ByteBuffer qbb = ByteBuffer.allocateDirect(arr.length * 4);
		// ����������nativeOrder
		qbb.order(ByteOrder.nativeOrder());
		mBuffer = qbb.asIntBuffer();
		mBuffer.put(arr);
		mBuffer.position(0);
		return mBuffer;
	}

	// ����һ�����߷�������float[]����ת��ΪOpenGL ES�����FloatBuffer
	private FloatBuffer floatBufferUtil(float[] arr)
	{
		FloatBuffer mBuffer;
		// ��ʼ��ByteBuffer������Ϊarr����ĳ���*4����Ϊһ��intռ4���ֽ�
		ByteBuffer qbb = ByteBuffer.allocateDirect(arr.length * 4);
		// ����������nativeOrder
		qbb.order(ByteOrder.nativeOrder());
		mBuffer = qbb.asFloatBuffer();
		mBuffer.put(arr);
		mBuffer.position(0);
		return mBuffer;
	}
}
