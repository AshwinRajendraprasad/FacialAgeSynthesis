#include "AgeingModel.h"

using namespace std;

const double TOL = 0.000000001;

bool matEqualWithinTol(cv::Mat a, cv::Mat b, double tol)
{
	if (a.size != b.size)
		return false;

	cv::Mat diff = a-b;
	bool eq = true;
	for (int i=0; i<a.rows; i++)
	{
		for (int j=0; j<a.cols; j++)
		{
			// If any element has too large error then not equal
			if (abs(diff.at<double>(i,j)) > TOL)
				eq = false;
		}
	}
	return eq;
}

// Loads the model from path and prints the fields expected to be there
void testLoading(string path)
{
	//TODO: Should list everything in each model
	//TODO: Should compare automatically - like testing fit

	// Testing that the model loads
	// The output of this is compared to the variables in Matlab - form of unit testing
	AgeingModel am = AgeingModel(path);

	cout << "AppModel.mean_texture = " << endl << *am.getAppModel().getField("mean_texture") << endl;
	cout << "AppModel.modes = " << endl << *am.getAppModel().getField("modes") << endl;
	cout << "AppModel.modes_inv = " << endl << *am.getAppModel().getField("modes_inv") << endl;
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

// Prints the output of fit image to model.  Needs to be checked for equality to Matlab answer
bool testFitImage(string path)
{
	AgeingModel test = AgeingModel(path);

	double testInput[4] = {1,2,3,4};

	cv::Mat texture = cv::Mat(1, 4, CV_64F, testInput);

	cv::Mat out = test.getAppModel().fitImageToAppModel(texture);

	double expOut[3] = {-2806175.867330770, -314181.1532544615, -4366434.742053692};
	cv::Mat exp = cv::Mat(3,1, CV_64F, expOut);

	// Check that the output is as expected
	bool eq = matEqualWithinTol(out, exp, TOL);

	//std::cout << "out: " << endl << out << endl;
	// Should be equal to:
	//  [-2806175.867330770; -314181.1532544615; -4366434.742053692]

	return eq;
}

bool testParams2Texture(string path)
{
	AgeingModel test = AgeingModel(path);

	double testInput[3] = {1,2,3};
	cv::Mat in = cv::Mat(3, 1, CV_64F, testInput);

	cv::Mat out = test.getAppModel().appParamsToTexture(in);

	double expOut[4] = {218794.1160000000, 84961.21992000000, 966.5527999999999, 5891.091120000001};
	cv::Mat exp = cv::Mat(4, 1, CV_64F, expOut);

	bool eq = matEqualWithinTol(out, exp, TOL);

	cout << "params2Texture: " << out << endl;

	return eq;
}

bool testAgeEst(string path)
{
	AgeingModel test = AgeingModel(path);

	double testInput[3] = {1,2,3};
	cv::Mat in = cv::Mat(3,1,CV_64F, testInput);
	double out = test.getAgeEstModel().predictAge(in);

	// Check whether output as expected
	double exp = 242.4736539139326;

	bool eq = true;
	if (abs(out-exp) > TOL)
		eq = false;

	//cout << "out: " << out << endl;

	return eq;
}

bool testAgeSynth(string path)
{
	AgeingModel test = AgeingModel(path);

	double testInput[3] = {1,2,3};
	cv::Mat in = cv::Mat(3,1,CV_64F, testInput);
	cv::Mat agedParams = test.changeFaceAge(in, 30, 'm');

	// Check whether output as expected
	double expOut[3] = {68.648889284532490, 221.1217764038630, 221.0997028491897};
	cv::Mat exp = cv::Mat(3,1, CV_64F, expOut);

	bool eq = matEqualWithinTol(agedParams, exp, TOL);

	return eq;
}

void main()
{
	testLoading("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");
	bool eqFit = testFitImage("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");
	bool eqP2T = testParams2Texture("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");
	bool eqEst = testAgeEst("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");
	bool eqSynth = testAgeSynth("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");

	cout << "Passed fitting to appearance model test: " << eqFit << endl;
	cout << "Passeed converting from appearance model parameters to texture test: " << eqP2T << endl;
	cout << "Passed age estimation test: " << eqEst << endl;
	cout << "Passed age synthesis test: " << eqSynth << endl;

	//AgeingModel test = AgeingModel("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test4");
	/*Model m = Model("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\image");
	cv::Mat texture = *m.getField("texture");*/

}
