#pragma once
#include "transmodel.h"
#include "FieldNames.h"

using namespace std;

class AgeEstModel :
	public TransModel
{
public:
	AgeEstModel(void);
	AgeEstModel(string path);
	~AgeEstModel(void);

	double predictAge(cv::Mat appParams);
};

