#include "stdafx.h"
#include "KLTTime.h"


KLTTime::KLTTime()
{
}


KLTTime::~KLTTime()
{
}

ostream& operator << (ostream& os, KLTTime& t) //定义运算符“<<”重载函数
{
	//os << t.detect << ',' << t.extract << ',' << t.matchkf << ',' << t.matchpt << ',' << t.ransac;
	return os;
}