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

cv::Mat AgeingModel::fitImageToAppModel(cv::Mat texture)
{
	/*Model m = Model("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\shape");
	cv::Mat neutralShape = *m.getField("neutral");
	m = Model("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Test face images");
	cv::Mat shape = *m.getField("shape");*/
	
	cv::Mat modes = *getAppModel().getField("modes");
	cv::Mat meanTexture = *getAppModel().getField("mean_texture");

	cv::Mat b = modes.t() * ((texture - getAppModel().getOffset())/getAppModel().getScale() - meanTexture).t();

	return b;
}

void AgeingModel::testLoading(string path)
{
	//TODO: Should list everything in each model

	// Testing that the model loads
	// The output of this is compared to the variables in Matlab - form of unit testing
	AgeingModel am = AgeingModel(path);

	cout << "AppModel.mean_texture = " << endl << *am.getAppModel().getField("mean_texture") << endl;
	cout << "AppModel.modes = " << endl << *am.getAppModel().getField("modes") << endl;
	cout << "AppModel.mask = " << endl << *am.getAppModel().getField("mask") << endl;
	cout << "AppModel.variances = " << endl << *am.getAppModel().getField("variances") << endl;
	cout << "AppModel.Transform.scale = " << endl << am.getAppModel().getScale() << endl;
	cout << "AppModel.Transform.offset = " << endl << am.getAppModel().getOffset() << endl << endl;

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

void main()
{
	AgeingModel::testLoading("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test3");

	/*AgeingModel am = AgeingModel("C:\\dataset\\Models\\Model3");
	Model m = Model("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\image");
	cv::Mat texture = *m.getField("texture");

	am.fitImageToAppModel(texture);*/
}