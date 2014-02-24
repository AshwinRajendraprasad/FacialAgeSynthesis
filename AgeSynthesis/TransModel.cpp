#include "TransModel.h"

const string SCALE = "scale";
const string OFFSET = "offset";

TransModel::TransModel(string path) : Model(path)
{
	map<string, cv::Mat*> transMap = LoadSingleModel(path + "\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat*>::const_iterator it = transMap.find(SCALE);
	transform.scale = it->second;
	it = transMap.find(OFFSET);
	transform.offset = it->second;
}

TransModel::~TransModel(void)
{
}

TransModel::Transform TransModel::getTransform()
{
	return transform;
}
