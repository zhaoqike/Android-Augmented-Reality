
#include "stdafx.h"
#include "Common.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#ifndef WIN32
#include <dirent.h>
#else
#include <Windows.h>
#endif

using namespace std;
using namespace cv;



bool isPrint = true;



//global timer to compute frame rate
Timer gtimer;

//global last frame time
double lastglbtime = 0;

//store frame rate
vector<double> frames;



bool CompareDI(DI a, DI b)
{
	return a.distance < b.distance;
}


vector<DMatch> FlipMatches(const vector<DMatch>& matches) {
	vector<DMatch> flip;
	for(int i=0;i<matches.size();i++) {
		flip.push_back(matches[i]);
		swap(flip.back().queryIdx,flip.back().trainIdx);
	}
	return flip;
}

vector<Point3d> CloudPointsToPoints(const vector<CloudPoint> cpts) {
	vector<Point3d> out;
	for (unsigned int i=0; i<cpts.size(); i++) {
		out.push_back(cpts[i].pt);
	}
	return out;
}

void GetAlignedPointsFromMatch(const vector<KeyPoint>& imgpts1,
							   const vector<KeyPoint>& imgpts2,
							   const vector<DMatch>& matches,
							   vector<KeyPoint>& pt_set1,
							   vector<KeyPoint>& pt_set2) 
{
	pt_set1.clear();
	pt_set2.clear();
	for (unsigned int i=0; i<matches.size(); i++) {
//		cout << "matches[i].queryIdx " << matches[i].queryIdx << " matches[i].trainIdx " << matches[i].trainIdx << endl;
		assert(matches[i].queryIdx < imgpts1.size());
		pt_set1.push_back(imgpts1[matches[i].queryIdx]);
		assert(matches[i].trainIdx < imgpts2.size());
		pt_set2.push_back(imgpts2[matches[i].trainIdx]);
	}	
}

void KeyPointsToPoints(const vector<KeyPoint>& kps, vector<Point2f>& ps) {
	ps.clear();
	for (unsigned int i=0; i<kps.size(); i++) ps.push_back(kps[i].pt);
}

void PointsToKeyPoints(const vector<Point2f>& ps, vector<KeyPoint>& kps) {
	kps.clear();
	for (unsigned int i=0; i<ps.size(); i++) kps.push_back(KeyPoint(ps[i],1.0f));
}

#define intrpmnmx(val,min,max) (max==min ? 0.0 : ((val)-min)/(max-min))

void drawArrows(Mat& frame, const vector<Point2f>& prevPts, const vector<Point2f>& nextPts, const vector<uchar>& status, const vector<float>& verror, const Scalar& _line_color)
{
	double minVal,maxVal; minMaxIdx(verror,&minVal,&maxVal,0,0,status);
	int line_thickness = 1;
	
    for (size_t i = 0; i < prevPts.size(); ++i)
    {
        if (status[i])
        {			
			double alpha = intrpmnmx(verror[i],minVal,maxVal); alpha = 1.0 - alpha;
			Scalar line_color(alpha*_line_color[0],
							  alpha*_line_color[1],
							  alpha*_line_color[2]);

            Point p = prevPts[i];
            Point q = nextPts[i];
            
            double angle = atan2((double) p.y - q.y, (double) p.x - q.x);
            
            double hypotenuse = sqrt( (double)(p.y - q.y)*(p.y - q.y) + (double)(p.x - q.x)*(p.x - q.x) );
            
            if (hypotenuse < 1.0)
                continue;
            
            // Here we lengthen the arrow by a factor of three.
            q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
            q.y = (int) (p.y - 3 * hypotenuse * sin(angle));
            
            // Now we draw the main line of the arrow.
            line(frame, p, q, line_color, line_thickness);
            
            // Now draw the tips of the arrow. I do some scaling so that the
            // tips look proportional to the main line of the arrow.
            
            p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
            p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
            line(frame, p, q, line_color, line_thickness);
            
            p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
            p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
            line(frame, p, q, line_color, line_thickness);
        }
    }
}

bool hasEnding (string const &fullString, string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool hasEndingLower (string const &fullString_, string const &_ending)
{
	string fullstring = fullString_, ending = _ending;
	transform(fullString_.begin(),fullString_.end(),fullstring.begin(),::tolower); // to lower
	return hasEnding(fullstring,ending);
}

void imshow_250x250(const string& name_, const Mat& patch) {
	Mat bigpatch; resize(patch,bigpatch,Size(250,250));
	imshow(name_,bigpatch);
}

void open_imgs_dir(string dir_name, vector<Mat>& images, vector<string>& images_names, double downscale_factor) {
	if (dir_name.length()==0) {
		return;
	}

	string dir_name_ = string(dir_name);
	vector<string> files_;

#ifndef WIN32
//open a directory the POSIX way

	DIR *dp;
	struct dirent *ep;     
	const char* dir_name_char=dir_name.c_str();
	dp = opendir (dir_name_char);
	
	if (dp != NULL)
	{
		while (ep = readdir (dp)) {
			if (ep->d_name[0] != '.')
				files_.push_back(ep->d_name);
		}
		
		(void) closedir (dp);
	}
	else {
		cerr << ("Couldn't open the directory");
		return;
	}

#else
//open a directory the WIN32 way
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fdata;

	if(dir_name_[dir_name_.size()-1] == '\\' || dir_name_[dir_name_.size()-1] == '/') {
		dir_name_ = dir_name_.substr(0,dir_name_.size()-1);
	}

	hFind = FindFirstFile(string(dir_name_).append("\\*").c_str(), &fdata);	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(fdata.cFileName, ".") != 0 &&
				strcmp(fdata.cFileName, "..") != 0)
			{
				if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					continue; // a diretory
				}
				else
				{
					files_.push_back(fdata.cFileName);
				}
			}
		}
		while (FindNextFile(hFind, &fdata) != 0);
	} else {
		cerr << "can't open directory\n";
		return;
	}

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		FindClose(hFind);
		cerr << "some other error with opening directory: " << GetLastError() << endl;
		return;
	}

	FindClose(hFind);
	hFind = INVALID_HANDLE_VALUE;
#endif
	
	for (unsigned int i=0; i<files_.size(); i++) {
		if (files_[i][0] == '.' || !(hasEndingLower(files_[i],"jpg")||hasEndingLower(files_[i],"png"))) {
			continue;
		}
		Mat m_ = imread(string(dir_name_).append("/").append(files_[i]));
		if(downscale_factor != 1.0)
			resize(m_,m_,Size(),downscale_factor,downscale_factor);
		images_names.push_back(files_[i]);
		images.push_back(m_);
	}
		

}

string intToString(int num)
{
	stringstream ss;
	string str;
	ss<<num;
	ss>>str;
	return str;
}



void redirectStdout(string path)
{
	time_t t;
	t = time(NULL);
	stringstream ss;
	string str;
	ss << t;
	ss >> str;

	string filename = path+"/out" + str + ".txt";

	freopen(filename.c_str(), "w", stdout);
}

void redirectStderr(string path)
{
	time_t t;
	t = time(NULL);
	stringstream ss;
	string str;
	ss << t;
	ss >> str;

	string filename = path + "/out" + str + ".txt";

	freopen(filename.c_str(), "w", stderr);
}

void CloudPointsToPoints(vector<CloudPoint>& cloudPoints, vector<Point3f>& points3d)
{
	points3d.reserve(cloudPoints.size());
	for (int i = 0; i < cloudPoints.size(); i++)
	{
		points3d.push_back(cloudPoints[i].pt);
	}
	
}






PMesh* makeMesh(string path)
{
	PMesh::EdgeCost g_edgemethod = PMesh::QUADRICTRI;
	const char* charPath = path.c_str();
	Mesh* mesh = new Mesh(charPath);
	//vector<vertex>& vert = g_pMesh->_vlist;

	if (mesh)
	{
		mesh->Normalize(0.2f);// center mesh around the origin & shrink to fit
	}
	conprint << "after normal" << endl;
	conprint << "  " << endl;

	PMesh* pmesh = new PMesh(mesh, g_edgemethod);
	return pmesh;
}


Model::Model(string path)
{
	pmesh = makeMesh(path);
	edgeNum = pmesh->numEdgeCollapses();
}

Model::Model()
{
	//meshIndex = 0;
}


