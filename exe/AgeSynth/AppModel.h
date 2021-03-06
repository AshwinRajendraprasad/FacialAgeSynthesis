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

	// Converts between a 2D image and a 1D texture - the texture only stores parts mask covers
	cv::Mat imageToTexture(cv::Mat image);
	cv::Mat textureToImage(cv::Mat texture);
	// Overloads that use a different mask to that from the model
	cv::Mat imageToTexture(cv::Mat image, cv::Mat mask);
	cv::Mat textureToImage(cv::Mat texture, cv::Mat mask);

private:
	// Only overriden to make private
	virtual Transform getTransform();
};

