#include "AppModel.h"

const string SCALE = "scale";
const string OFFSET = "translate";

AppModel::AppModel(void)
{
}

AppModel::AppModel(string path) : TransModel(path)
{
	cout << "Loading app.transform model" << endl;
	map<string, cv::Mat*> transMap = LoadSingleModel(path + "\\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat*>::const_iterator it = transMap.find(SCALE);
	cv::Mat scale = *it->second;
	it = transMap.find(OFFSET);
	cv::Mat offset = *it->second;

	setTransform(scale, offset);
}


AppModel::~AppModel(void)
{
}

double AppModel::getScale()
{
	// There is only one value in this, so return it
	return getTransform().getScale().at<double>(0,0);
}

double AppModel::getOffset()
{
	// There is only one value in this, so return it
	return getTransform().getOffset().at<double>(0,0);
}

AppModel::Transform AppModel::getTransform()
{
	return TransModel::getTransform();
}
