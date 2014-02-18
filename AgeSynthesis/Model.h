#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include <cv.h>

#include <vector>

class Model
{
public:
	Model(void);
	~Model(void);
	void LoadModel(std::string path);

	void LoadSingleModel(std::string path);
};

struct Field {
	std::string name;
	cv::Mat value;
};

