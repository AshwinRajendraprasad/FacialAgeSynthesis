#include "AgeSynthModel.h"


AgeSynthModel::AgeSynthModel(string path) : Model(path)
{
	male = GenderModel(path+"\\male");
	female = GenderModel(path+"\\female");
}

AgeSynthModel::AgeSynthModel(void)
{
}

AgeSynthModel::~AgeSynthModel(void)
{
}

// Getters
AgeSynthModel::GenderModel AgeSynthModel::getMale()
{
	return male;
}

AgeSynthModel::GenderModel AgeSynthModel::getFemale()
{
	return female;
}

// GenderModel functions
#pragma region

AgeSynthModel::GenderModel::GenderModel(void)
{
}

AgeSynthModel::GenderModel::GenderModel(string path)
{
	// Load the older/younger models
	older = Model(path+"\\older");
	younger = Model(path+"\\younger");
}

Model AgeSynthModel::GenderModel::getYounger()
{
	return younger;
}
Model AgeSynthModel::GenderModel::getOlder()
{
	return older;
}
#pragma endregion