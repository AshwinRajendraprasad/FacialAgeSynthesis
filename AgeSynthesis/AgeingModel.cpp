#include "AgeingModel.h"


AgeingModel::AgeingModel(string path)
{
	string p = path+"\\App";
	appModel = TransModel(p);
	ageEstModel = TransModel(path+"\\AgeEst");
	ageSynthModel = AgeSynthModel(path+"\\AgeSynth");
}

AgeingModel::AgeingModel(void)
{
}

AgeingModel::~AgeingModel(void)
{
}

// Getters
TransModel AgeingModel::getAppModel()
{
	return appModel;
}

TransModel AgeingModel::getAgeEstModel()
{
	return ageEstModel;
}

AgeSynthModel AgeingModel::getAgeSynthModel()
{
	return ageSynthModel;
}

void main()
{
	// Testing that the model loads
	// The output of this is compared to the variables in Matlab - form of unit testing
	AgeingModel am = AgeingModel("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test1");

	cout << "AppModel.mean_texture = " << endl << *am.getAppModel().getField("mean_texture") << endl;
	cout << "AppModel.modes = " << endl << *am.getAppModel().getField("modes") << endl;
	cout << "AppModel.variances = " << endl << *am.getAppModel().getField("variances") << endl;
	cout << "AppModel.Transform.scale = " << endl << am.getAppModel().getTransform().getScale() << endl;
	cout << "AppModel.Transform.offset = " << endl << am.getAppModel().getTransform().getOffset() << endl << endl;

	cout << "AgeEst.offset = " << endl << *am.getAgeEstModel().getField("offset") << endl;
	cout << "AgeEst.coeffs = " << endl << *am.getAgeEstModel().getField("coeffs") << endl;
	cout << "AgeEst.Transform.scale = " << endl << am.getAgeEstModel().getTransform().getScale() << endl;
	cout << "AgeEst.Transform.offset = " << endl << am.getAgeEstModel().getTransform().getOffset() << endl << endl;

	cout << "AgeSynth.male.younger.del_b = " << endl << *am.getAgeSynthModel().getMale().getYounger().getField("del_b") << endl;
	cout << "AgeSynth.male.younger.fac = " << endl << *am.getAgeSynthModel().getMale().getYounger().getField("fac") << endl;
	cout << "AgeSynth.male.older.del_b = " << endl << *am.getAgeSynthModel().getMale().getOlder().getField("del_b") << endl;
	cout << "AgeSynth.male.older.fac = " << endl << *am.getAgeSynthModel().getMale().getOlder().getField("fac")<< endl;
	cout << "AgeSynth.female.younger.del_b = " << endl << *am.getAgeSynthModel().getFemale().getYounger().getField("del_b") << endl;
	cout << "AgeSynth.female.younger.fac = " << endl << *am.getAgeSynthModel().getFemale().getYounger().getField("fac") << endl;
	cout << "AgeSynth.female.older.del_b = " << endl << *am.getAgeSynthModel().getFemale().getOlder().getField("del_b") << endl;
	cout << "AgeSynth.female.older.fac = " << endl << *am.getAgeSynthModel().getFemale().getOlder().getField("fac")<< endl;
}