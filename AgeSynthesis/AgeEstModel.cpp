#include "AgeEstModel.h"

AgeEstModel::AgeEstModel(void)
{
}

AgeEstModel::AgeEstModel(string path) : TransModel(path)
{
	cout << "Loading ageEst.transform model" << endl;
	map<string, cv::Mat> transMap = LoadSingleModelB(path + "\\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat>::const_iterator it = transMap.find(FieldNames::EST_TRANS_SCALE);
	cv::Mat scale = it->second;
	it = transMap.find(FieldNames::EST_TRANS_OFFSET);
	cv::Mat offset = it->second;

	setTransform(scale, offset);
}


AgeEstModel::~AgeEstModel(void)
{
}

double AgeEstModel::predictAge(cv::Mat appParams)
{
	cv::divide(appParams - getTransform().getOffset(), getTransform().getScale(), appParams);
	cv::Mat sqParams;
	cv::multiply(appParams, appParams, sqParams);
	cv::Mat age = getField(FieldNames::EST_OFFSET) + getField(FieldNames::EST_COEFFS).row(0)*appParams + getField(FieldNames::EST_COEFFS).row(1)*sqParams;
	if (age.size().height == 1 && age.size().width == 1)
		return age.at<double>(0,0);
	else
		// This can't happen!
		throw "Error!";
}
