#pragma once
#include "AgeSynthModel.h"
#include "TransModel.h"

using namespace std;

class AgeingModel
{
public:
	AgeingModel(void);
	AgeingModel(string path);
	~AgeingModel(void);

	TransModel getAppModel();
	TransModel getAgeEstModel();
	AgeSynthModel getAgeSynthModel();

private:
	TransModel appModel;
	TransModel ageEstModel;
	AgeSynthModel ageSynthModel; 
};

