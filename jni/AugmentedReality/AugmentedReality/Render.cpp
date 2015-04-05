#include "StdAfx.h"
#include "Render.h"


Render::Render(void)
{
}


Render::~Render(void)
{
}

void Render::FilledCircle(Mat& img, Point2f& center,Scalar& scalar)
{
	int thickness = -1;
	int lineType = 8;

	circle(img,
		center,
		2.0,
		scalar,
		thickness,
		lineType);
}

void Render::Line(Mat& img, Point2f& start, Point2f& end)
{
	int thickness = 1;
	int lineType = 8;
	line(img, start, end, Scalar(125, 125, 125), thickness);
	/*line(img,
		start,
		end,
		Scalar(0, 0, 0),
		thickness,
		lineType);*/
}