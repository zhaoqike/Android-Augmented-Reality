#include "Common.h"

bool captureFlag=false;


osg::ref_ptr<osg::Camera> createHUDBg(osg::ref_ptr<osg::Image> image){
	osg::ref_ptr<osg::Camera>camera = new osg::Camera;
	camera->setProjectionMatrixAsOrtho2D(0, image->s(), 0, image->t());
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setViewport(0, 0, image->s(), image->t());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus(false);
	camera->setViewMatrix(osg::Matrix::identity());

	osg::ref_ptr<osg::Geode>geode = new osg::Geode;
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Geometry>gm = new osg::Geometry;

	//亚入顶点
	osg::ref_ptr<osg::Vec3Array>vertex = new osg::Vec3Array;
	vertex->push_back(osg::Vec3(0, 0, 0));
	vertex->push_back(osg::Vec3(image->s(), 0, 0));
	vertex->push_back(osg::Vec3(image->s(), image->t(), 0));
	vertex->push_back(osg::Vec3(0, image->t(), 0));
	gm->setVertexArray(vertex);

	//压入法线

	//纹理坐标
	osg::ref_ptr<osg::Vec2Array>coord = new osg::Vec2Array;
	coord->push_back(osg::Vec2(0, 0));
	coord->push_back(osg::Vec2(1, 0));
	coord->push_back(osg::Vec2(1, 1));
	coord->push_back(osg::Vec2(0, 1));
	gm->setTexCoordArray(0, coord);
	gm->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//osg::ref_ptr<osg::Image>image=osgDB::readImageFile(imagePath);
	if (!image.valid()){
		//    std::cout<"read image faild "<<std::endl;
		return camera.release();
	}
	//  osg::ref_ptr<osg::Texture2D>t2d=new osg::Texture2D;
	osg::Texture2D*t2d = new osg::Texture2D;
	t2d->setImage(0, image);



	//BackGroundCallBack cb1;
	//osg::ref_ptr<osg::StateAttributeCallback> cb = new BackGroundCallBack();
	//t2d->setUpdateCallback(new BackGroundCallBack());


	gm->getOrCreateStateSet()->setTextureAttributeAndModes(0, t2d, osg::StateAttribute::ON);
	geode->addDrawable(gm);
	camera->addChild(geode);
	return camera.release();
}

osg::ref_ptr<osg::Camera> createHUDBg()
{
	Mat image;
	image = cv::imread("sdcard/aa.jpg");
	cv::imwrite("sdcard/cvwrite1.bmp", image);
	osg::ref_ptr<osg::Image> osgImage=matToOsgImage(image);
	osg::ref_ptr<osg::Camera> camera = createHUDBg(osgImage);
	Mat copy=osgImageToMat(osgImage);
	cv::imwrite("sdcard/cvwrite2.bmp", copy);
	return camera;
}

void readwrite()
{
	Mat image;
	image = cv::imread("sdcard/abcd.bmp");
	cv::imwrite("sdcard/cvaaaaa.bmp", image);
}

osg::ref_ptr<osg::Image> matToOsgImage(Mat& mat)
{
	Mat rgb;
	cvtColor(mat, rgb, CV_BGR2RGB);
	flip(rgb, rgb, 0);

	osg::ref_ptr<osg::Image> image = new osg::Image;

	uchar *data = new uchar[rgb.total()*rgb.elemSize()];
	memcpy(data, rgb.data, rgb.total()*rgb.elemSize());
	image->setImage(mat.size().width, mat.size().height, 1, GL_RGBA32F_ARB, GL_RGB, GL_UNSIGNED_BYTE, data, osg::Image::AllocationMode::NO_DELETE);
	return image;
}

Mat osgImageToMat(osg::ref_ptr<osg::Image> image)
{
	cout << "fuck" << endl;
	int width = image->s();
	int height = image->t();
	int depth = image->r();
	cout << "fuck1" << endl;
	int nchannels = (int)((image->getPixelSizeInBits()) == 1 ? 1 : (image->getPixelSizeInBits()) / (8 * image->r()));
	IplImage* imgIpl = cvCreateImage(cvSize(image->s(), image->t()), 8 * image->r(), nchannels);
	cout << "after create image" << endl;
	cout << "before origin" << endl;
	//CImage2IplImage(image, imgIpl);//该函数为将OSG图像转换为OpenCV图像
	imgIpl->origin = IPL_ORIGIN_BL;
	cout << "after origin" << endl;
	cout << "fuck2" << endl;
	cout << IPL_ORIGIN_BL << endl;
	memcpy((unsigned char*)imgIpl->imageData, image->data(), image->getImageSizeInBytes());
	cout << "after memcpy" << endl;
	cout << "image get size in byte "<<image->getImageSizeInBytes() << endl;

	cout << "osg image to mat: " << width << "  " << height << "  " << depth << "  "<<nchannels<<endl;


	cout << width << "  " << height << "  " << depth << endl;
	Mat mat(imgIpl);
	cvtColor(mat, mat, CV_RGB2BGR);
	flip(mat, mat, 0);
	return mat;
}

void mergeImage(Mat& background,Mat& model)
{
	int rows = background.rows;
	int cols = background.cols;
	resize(model, model, background.size());
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Vec3b dot = model.at<Vec3b>(i, j);
			if (dot[0] == 0 && dot[1] == 0 && dot[2] == 0)
			{

			}
			else
			{
				background.at<Vec3b>(i, j) = model.at<Vec3b>(i, j);
			}
		}
	}
	return;
}
