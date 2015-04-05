/*
 * Map
 *
 *  Created on: 2014��6��4��
 *      Author: zhaoqike
 */

#ifndef MAP_
#define MAP_
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <iostream>



#include "Timer.h"

using namespace std;

class Map
{
public:
	int findfeature(long long addrGray, long long addrRgba);
	int test();
	void changeColor();
};


#endif /* MAP_ */
