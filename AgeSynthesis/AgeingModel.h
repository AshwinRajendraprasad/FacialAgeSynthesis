#pragma once
#include "AgeSynthModel.h"
#include "AgeEstModel.h"
#include "AppModel.h"
#include "FieldNames.h"

using namespace std;

class AgeingModel
{
public:
	AgeingModel(void);
	AgeingModel(string path);
	~AgeingModel(void);

	AppModel getAppModel();
	AgeEstModel getAgeEstModel();
	AgeSynthModel getAgeSynthModel();

	// The functions needed to perform age synthesis
	cv::Mat changeFaceAge(cv::Mat appParams, int targetAge, char gender);

	enum CompOper { LESS_THAN, GREATER_THAN };


private:
	AppModel appModel;
	AgeEstModel ageEstModel;
	AgeSynthModel ageSynthModel;

	// Clamps 1D column vector.  The operator is when a value is outside the required range
	cv::Mat AgeingModel::clamp(cv::Mat in, cv::Mat clampVals, AgeingModel::CompOper oper);

};

