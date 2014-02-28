#pragma once
#include "transmodel.h"
#include "FieldNames.h"

using namespace std;

class AppModel :
	public TransModel
{
public:
	AppModel(void);
	AppModel(string path);
	~AppModel(void);

	double getScale();
	double getOffset();

	// Converting between appearance parameters and texture
	cv::Mat fitImageToAppModel(cv::Mat texture);
	cv::Mat appParamsToTexture(cv::Mat appParams);

private:
	// Only overriden to make private
	virtual Transform getTransform();
};

