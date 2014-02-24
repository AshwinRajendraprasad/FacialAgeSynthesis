#include "TransModel.h"

const string SCALE = "scale";
const string OFFSET = "offset";

TransModel::TransModel(string path) : Model(path)
{
	map<string, cv::Mat*> transMap = LoadSingleModel(path + "\\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat*>::const_iterator it = transMap.find(SCALE);
	double scale = it->second->at<double>(0,0);  // This will only have one element which is a double
	it = transMap.find(OFFSET);
	double offset = it->second->at<double>(0,0);  // This will only have one element which is a double

	transform = Transform(scale, offset);
}

TransModel::TransModel(void)
{
}

TransModel::~TransModel(void)
{
}

TransModel::Transform TransModel::getTransform()
{
	return transform;
}

// Transform functions
#pragma region
TransModel::Transform::Transform(void)
{
}

TransModel::Transform::Transform(double s, double o)
{
	scale = s;
	offset = o;
}

double TransModel::Transform::getScale()
{
	return scale;
}
double TransModel::Transform::getOffset()
{
	return offset;
}
#pragma endregion
