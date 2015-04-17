#pragma once
class KLTTime
{
public:
	KLTTime();
	~KLTTime();
	friend ostream& operator<<(ostream& os, KLTTime& t);
};

