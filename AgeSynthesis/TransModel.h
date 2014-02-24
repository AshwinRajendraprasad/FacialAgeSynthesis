#pragma once
#include "model.h"

class TransModel :
	public Model
{
public:
	TransModel(string path);
	~TransModel(void);

	struct Transform
	{
		cv::Mat* scale;
		cv::Mat* offset;
	};

	Transform getTransform();

private:
	Transform transform;
};

