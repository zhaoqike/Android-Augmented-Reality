#pragma once
class MyKeyPoint
{
public:
	MyKeyPoint();
	MyKeyPoint(KeyPoint k);
	~MyKeyPoint();
	KeyPoint kpt;
	int i;

	void write(FileStorage& fs) const;
	void read(const FileNode& node);
};

