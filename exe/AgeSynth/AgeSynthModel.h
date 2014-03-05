#pragma once
#include "model.h"

using namespace std;

class AgeSynthModel :
	public Model
{
public:
	AgeSynthModel(void);
	AgeSynthModel(string path);
	~AgeSynthModel(void);

	struct GenderModel
	{
	public:
		GenderModel(void);
		GenderModel(string path);

		Model getOlder();
		Model getYounger();

	private:
		Model older;
		Model younger;
	};

	GenderModel getMale();
	GenderModel getFemale();

private:
	GenderModel male;
	GenderModel female;
};

