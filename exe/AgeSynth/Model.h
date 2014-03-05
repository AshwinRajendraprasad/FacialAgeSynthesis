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
	Model(string path);
	~Model(void);

	map<string, cv::Mat> LoadSingleModel(std::string path);
	// dealing with binary files to attempt to speed up loading
	map<string, cv::Mat> LoadSingleModelB(std::string path);
	cv::Mat getField(string name);

private:
	int bytes2int(char* bytes);
	double bytes2double(char* bytes);
	map<string, cv::Mat> fields;
};
