#include "stdafx.h"
#include "KLTTime.h"


KLTTime::KLTTime()
{
}


KLTTime::~KLTTime()
{
}

ostream& operator << (ostream& os, KLTTime& t) //�����������<<�����غ���
{
	//os << t.detect << ',' << t.extract << ',' << t.matchkf << ',' << t.matchpt << ',' << t.ransac;
	return os;
}