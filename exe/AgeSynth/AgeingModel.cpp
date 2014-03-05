#include "AgeingModel.h"

AgeingModel::AgeingModel(string path)
{
	appModel = AppModel(path+"\\App");
	ageEstModel = AgeEstModel(path+"\\AgeEst");
	ageSynthModel = AgeSynthModel(path+"\\AgeSynth");
}

AgeingModel::AgeingModel(void)
{
}

AgeingModel::~AgeingModel(void)
{
}

// Getters
AppModel AgeingModel::getAppModel()
{
	return appModel;
}

AgeEstModel AgeingModel::getAgeEstModel()
{
	return ageEstModel;
}

AgeSynthModel AgeingModel::getAgeSynthModel()
{
	return ageSynthModel;
}


cv::Mat AgeingModel::changeFaceAge(cv::Mat appParams, int targetAge, char gender)
{
	// Get the correct gender model
	AgeSynthModel::GenderModel simple;
	if (gender=='m')
		simple = getAgeSynthModel().getMale();
	else if (gender == 'f')
		simple = getAgeSynthModel().getFemale();
	else
		throw "Gender must be either 'm' or 'f'";

	// Add on the correct amount of the correct del_b to change the face parameters
	double del_age = targetAge - getAgeEstModel().predictAge(appParams);
	cv::Mat agedParams;
	if (del_age < 0)
	{
		cv::Mat factor = simple.getYounger().getField(FieldNames::FAC);
		// Factor is just one value
		agedParams = appParams + (-del_age * factor.at<double>(0,0)) * simple.getYounger().getField(FieldNames::DEL_B);
	}
	else
	{
		cv::Mat factor = simple.getOlder().getField(FieldNames::FAC);
		// Factor is just one value
		agedParams = appParams + (del_age * factor.at<double>(0,0)) * simple.getOlder().getField(FieldNames::DEL_B);
	}

	// Clamp the model parameters so they are within 3 standard deviations
	cv::Mat stddev;
	cv::sqrt(getAppModel().getField(FieldNames::APP_VARIANCES), stddev);
	agedParams = clamp(agedParams, stddev*3, GREATER_THAN);
	agedParams = clamp(agedParams, stddev*-3, LESS_THAN);

	return agedParams;
}

cv::Mat AgeingModel::clamp(cv::Mat in, cv::Mat clampVals, AgeingModel::CompOper oper)
{
	// Find which elements need changing based on the operator specified
	cv::Mat comp;
	switch (oper)
	{
	case LESS_THAN:
		comp = in < clampVals;
		break;
	case GREATER_THAN:
		comp = in > clampVals;
		break;
	}

	for (int i=0; i<in.rows; i++)
	{
		// comp is 255 when need to change element
		if (comp.at<char>(i,0) == char(255))
		{
			in.at<double>(i,0) = clampVals.at<double>(i,0);
		}
	}

	return in;
}