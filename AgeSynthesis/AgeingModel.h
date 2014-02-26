#pragma once
#include "AgeSynthModel.h"
#include "AgeEstModel.h"
#include "AppModel.h"

//#include <PAW.h>

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

	cv::Mat fitImageToAppModel(cv::Mat texture);

	// Loads the model from path and prints the fields expected to be there
	static void testLoading(string path);

private:
	AppModel appModel;
	AgeEstModel ageEstModel;
	AgeSynthModel ageSynthModel; 
};

