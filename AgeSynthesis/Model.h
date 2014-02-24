#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <cv.h>
#include <vector>

#include "Mat.h"

using namespace std;

class Model
{
public:
	Model(void);
	~Model(void);

	map<string, cv::Mat* > LoadSingleModel(std::string path);
};
