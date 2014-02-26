#pragma once
#include "transmodel.h"

using namespace std;

class AgeEstModel :
	public TransModel
{
public:
	AgeEstModel(void);
	AgeEstModel(string path);
	~AgeEstModel(void);
};

