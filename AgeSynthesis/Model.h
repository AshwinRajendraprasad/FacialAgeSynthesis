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
	Model(string path);
	~Model(void);

	map<string, cv::Mat*> LoadSingleModel(std::string path);
	cv::Mat* getField(string name);

private:
	map<string, cv::Mat*> fields;
};
