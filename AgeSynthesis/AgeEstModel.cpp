#include "AgeEstModel.h"

const string SCALE = "scale";
const string OFFSET = "offset";

AgeEstModel::AgeEstModel(void)
{
}

AgeEstModel::AgeEstModel(string path) : TransModel(path)
{
	cout << "Loading ageEst.transform model" << endl;
	map<string, cv::Mat*> transMap = LoadSingleModel(path + "\\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat*>::const_iterator it = transMap.find(SCALE);
	cv::Mat scale = *it->second;
	it = transMap.find(OFFSET);
	cv::Mat offset = *it->second;

	setTransform(scale, offset);
}


AgeEstModel::~AgeEstModel(void)
{
}
