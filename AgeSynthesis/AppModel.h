#pragma once
#include "transmodel.h"

using namespace std;

class AppModel :
	public TransModel
{
public:
	AppModel(void);
	AppModel(string path);
	~AppModel(void);

	double getScale();
	double getOffset();

private:
	// Only overriden to make private
	virtual Transform getTransform();
};

