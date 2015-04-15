#include "stdafx.h"

#include "YamlStorage.h"



///store
void write(FileStorage& fs, const string&, const KeyFrame& keyframe)
{
	keyframe.write(fs);
}
void read(const FileNode& node, KeyFrame& keyframe, const KeyFrame& default_value)
{
	if (node.empty())
	{
		keyframe = default_value;
	}
	else
	{
		keyframe.read(node);
	}
}

void write(FileStorage& fs, const string&, const CloudPoint& cp)
{
	cp.write(fs);
}
void read(const FileNode& node, CloudPoint& cp, const CloudPoint& default_value)
{
	if (node.empty())
	{
		cp = default_value;
	}
	else
	{
		cp.read(node);
	}
}


void write(FileStorage& fs, const string&, const MyKeyPoint& m)
{
	m.write(fs);
}

void read(const FileNode& node, MyKeyPoint& m, const MyKeyPoint& default_value)
{
	if (node.empty())
	{
		m = default_value;
	}
	else
	{
		m.read(node);
	}
}

void write(FileStorage& fs, const string&, const vector<MyKeyPoint>& m)
{
	fs << "[";
	for (int i = 0; i < m.size(); i++)
	{
		m[i].write(fs);
	}
	fs << "]";
}




void read(const FileNode& node, vector<MyKeyPoint>& m, const vector<MyKeyPoint>& default_value)
{
	if (node.empty())
	{
		m = default_value;
	}
	else
	{
		FileNodeIterator it = node.begin(), it_end = node.end();
		for (; it != it_end; ++it)
		{
			MyKeyPoint d;
			d.read(*it);
			m.push_back(d);
		}
	}
}

void write(FileStorage& fs, const string&, const vector<KeyFrame>& m)
{
	fs << "[";
	for (int i = 0; i < m.size(); i++)
	{
		m[i].write(fs);
	}
	fs << "]";
}




void read(const FileNode& node, vector<KeyFrame>& m, const vector<KeyFrame>& default_value)
{
	if (node.empty())
	{
		m = default_value;
	}
	else
	{
		FileNodeIterator it = node.begin(), it_end = node.end();
		for (; it != it_end; ++it)
		{
			KeyFrame d;
			d.read(*it);
			m.push_back(d);
		}
	}
}

void write(FileStorage& fs, const string&, const vector<CloudPoint>& m)
{
	fs << "[";
	for (int i = 0; i < m.size(); i++)
	{
		m[i].write(fs);
	}
	fs << "]";
}




void read(const FileNode& node, vector<CloudPoint>& m, const vector<CloudPoint>& default_value)
{
	if (node.empty())
	{
		m = default_value;
	}
	else
	{
		FileNodeIterator it = node.begin(), it_end = node.end();
		for (; it != it_end; ++it)
		{
			CloudPoint d;
			d.read(*it);
			m.push_back(d);
		}
	}
}