#include <iostream>
#include "Common.h"
using namespace std;

extern bool captureFlag;


/** Capture the frame buffer and write image to disk*/
class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:
	WindowCaptureCallback(GLenum readBuffer, const std::string& name) :
		_readBuffer(readBuffer),
		_fileName(name)
	{
		_image = new osg::Image;
	}

	virtual void operator () (osg::RenderInfo& renderInfo) const
	{
		if (captureFlag == false)
		{
			return;
		}
		cout << "begin capture" << endl;
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
		glReadBuffer(_readBuffer);
#else
		osg::notify(osg::NOTICE) << "Error: GLES unable to do glReadBuffer" << std::endl;
#endif

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
		if (gc->getTraits())
		{
			GLenum pixelFormat;

			if (gc->getTraits()->alpha)
				pixelFormat = GL_RGBA;
			else
				pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
			if (pixelFormat == GL_RGB)
			{
				GLint value = 0;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
				glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
				if (value != GL_RGB ||
					value != GL_UNSIGNED_BYTE)
				{
					pixelFormat = GL_RGBA;//always supported
				}
			}
#endif
			int width = gc->getTraits()->width;
			int height = gc->getTraits()->height;

			std::cout << "Capture: size=" << width << "x" << height << ", format=" << (pixelFormat == GL_RGBA ? "GL_RGBA" : "GL_RGB") << std::endl;

			_image->readPixels(0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE);
		}

		if (!_fileName.empty())
		{
			std::cout << "Writing to: " << _fileName << std::endl;
			osgDB::writeImageFile(*_image, _fileName);
			Mat cvimg = osgImageToMat(_image);
			cv::imwrite("sdcard/outcv.bmp", cvimg);

			Mat bg;
			bg = cv::imread("sdcard/aa.jpg");
			cv::imwrite("sdcard/cvwrite1.bmp", bg);

		}
		captureFlag = false;
		cout << "end capture" << endl;
	}

protected:
	GLenum                      _readBuffer;
	std::string                 _fileName;
	osg::ref_ptr<osg::Image>    _image;
	mutable OpenThreads::Mutex  _mutex;
};
