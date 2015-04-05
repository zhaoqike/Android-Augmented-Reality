#pragma once
class Render
{
public:
	Render(void);
	~Render(void);
	void FilledCircle(Mat& img, Point2f& center, Scalar& scalar);
	void Line(Mat& img, Point2f& start, Point2f& end);
};

