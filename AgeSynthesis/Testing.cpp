#include "AgeingModel.h"
#include "FieldNames.h"

#include <cv.h>
#include <opencv2\highgui\highgui.hpp>

using namespace std;

const double TOL = 0.000000001;

bool matEqualWithinTol(cv::Mat a, cv::Mat b, double tol)
{
	if (a.size != b.size)
		return false;

	bool eq = true;
	
	for (int i=0; i<a.rows; i++)
	{
		for (int j=0; j<a.cols; j++)
		{
			if (a.type() == CV_64FC3)
			{
				// Need to check all 3 elements of the vector
				for (int x=0; x<3; x++)
				{
					double m = a.at<cv::Vec3d>(i,j)[x];
					double n = b.at<cv::Vec3d>(i,j)[x];
					if (abs(a.at<cv::Vec3d>(i,j)[x] - b.at<cv::Vec3d>(i,j)[x]) > TOL)
						eq = false;
				}
			}
			else
			{
				// If any element has too large error then not equal
				if (abs(a.at<double>(i,j) - b.at<double>(i,j)) > TOL)
					eq = false;
			}
		}
	}
	return eq;
}

// Loads the model from path and prints the fields expected to be there
bool testLoading(string path)
{

	// Testing that the model loads
	// The output of this is compared to the variables in Matlab - form of unit testing
	AgeingModel am = AgeingModel(path);

	bool eq = true;

	// Appearance model
	double meanT[4] = {54.32, 5234.5423, 63.532, 543.6453};
	cv::Mat expMeanT = cv::Mat(1, 4, CV_64F, meanT);
	eq = eq && matEqualWithinTol(am.getAppModel().getField(FieldNames::APP_MEAN_T), expMeanT, TOL);

	double modes[4][3] = {{45.36, 653.53, 6543.35}, {534.6, 52.62, 764.62}, {3.4, 7.2, 3.5}, {8.4, 3.8, 1.9}};
	cv::Mat expModes = cv::Mat(4, 3, CV_64F, modes);
	eq = eq && matEqualWithinTol(am.getAppModel().getField(FieldNames::APP_MODES), expModes, TOL);

	double variances[3] = {523.63, 5432.76, 5431.6754};
	cv::Mat expVar = cv::Mat(3, 1, CV_64F, variances);
	eq = eq && matEqualWithinTol(am.getAppModel().getField(FieldNames::APP_VARIANCES), expVar, TOL);

	cv::Mat expModesInv = expModes.t();
	eq = eq && matEqualWithinTol(am.getAppModel().getField(FieldNames::APP_MODES_INV), expModesInv, TOL);

	double mask[3][4] = {{0,1,1,0}, {0,0,1,0}, {1,0,1,0}};
	cv::Mat expMask = cv::Mat(3, 4, CV_64F, mask);
	eq = eq && matEqualWithinTol(am.getAppModel().getField(FieldNames::APP_MASK), expMask, TOL);

	double expAppScale = 10.4;
	eq = eq && (abs(expAppScale - am.getAppModel().getScale()) < TOL);
	double expAppOffset = 11.5;
	eq = eq && (abs(expAppOffset - am.getAppModel().getOffset()) < TOL);


	// Age est model
	double expOffset = 214.643;
	eq = eq && (abs(expOffset - am.getAgeEstModel().getField(FieldNames::EST_OFFSET).at<double>(0,0)) < TOL);

	double coeffs[2][3] = {{543.3630, 543.5340, 324}, {435.2340, 534.63, 5423.5}};
	cv::Mat expCoeffs = cv::Mat(2, 3, CV_64F, coeffs);
	eq = eq && matEqualWithinTol(am.getAgeEstModel().getField(FieldNames::EST_COEFFS), expCoeffs, TOL);

	double tranScale[3] = {4.6,3.3,7.9};
	cv::Mat expTransScale = cv::Mat(3, 1, CV_64F, tranScale);
	eq = eq && matEqualWithinTol(am.getAgeEstModel().getTransform().getScale(), expTransScale, TOL);

	double tranOffset[3] = {2.8,1.9,4.6};
	cv::Mat expTransOffset = cv::Mat(3, 1, CV_64F, tranOffset);
	eq = eq && matEqualWithinTol(am.getAgeEstModel().getTransform().getOffset(), expTransOffset, TOL);


	// Age synth model
	double myDelB[3] = {543.25, 5436.432, 5423.87};
	cv::Mat expMYDelB = cv::Mat(3, 1, CV_64F, myDelB);
	eq = eq && matEqualWithinTol(am.getAgeSynthModel().getMale().getYounger().getField(FieldNames::DEL_B), expMYDelB, TOL);

	double expMYFac = 4.65;
	eq = eq && (abs(expMYFac - am.getAgeSynthModel().getMale().getYounger().getField(FieldNames::FAC).at<double>(0,0)) < TOL);

	double moDelB[3] = {132.25, 12.432, 456.87};
	cv::Mat expMODelB = cv::Mat(3, 1, CV_64F, moDelB);
	eq = eq && matEqualWithinTol(am.getAgeSynthModel().getMale().getOlder().getField(FieldNames::DEL_B), expMODelB, TOL);

	double expMOFac = 7.34;
	eq = eq && (abs(expMOFac - am.getAgeSynthModel().getMale().getOlder().getField(FieldNames::FAC).at<double>(0,0)) < TOL);

	double fyDelB[3] = {43.25, 573.432, 754.87};
	cv::Mat expFYDelB = cv::Mat(3, 1, CV_64F, fyDelB);
	eq = eq && matEqualWithinTol(am.getAgeSynthModel().getFemale().getYounger().getField(FieldNames::DEL_B), expFYDelB, TOL);

	double expFYFac = 1.65;
	eq = eq && (abs(expFYFac - am.getAgeSynthModel().getFemale().getYounger().getField(FieldNames::FAC).at<double>(0,0)) < TOL);

	double foDelB[3] = {79.25, 46.432, 46312.87};
	cv::Mat expFODelB = cv::Mat(3, 1, CV_64F, foDelB);
	eq = eq && matEqualWithinTol(am.getAgeSynthModel().getFemale().getOlder().getField(FieldNames::DEL_B), expFODelB, TOL);

	double expFOFac = 3143.654364861234;
	eq = eq && (abs(expFOFac - am.getAgeSynthModel().getFemale().getOlder().getField(FieldNames::FAC).at<double>(0,0)) < TOL);

	return eq;
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

	//cout << "params2Texture: " << out << endl;

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

bool testImageTextureConv(string path)
{
	AgeingModel test = AgeingModel(path);

	bool eq = true;

	// Input would have zeros where mask is 0 as thats how it would be made
	cv::Mat in = cv::Mat::zeros(3, 4, CV_8UC3);
	in.at<cv::Vec3b>(0,1) = cv::Vec3b(2,2,2);
	in.at<cv::Vec3b>(0,2) = cv::Vec3b(3,3,3);
	in.at<cv::Vec3b>(1,2) = cv::Vec3b(7,7,7);
	in.at<cv::Vec3b>(2,2) = cv::Vec3b(3,3,3);
	in.at<cv::Vec3b>(2,0) = cv::Vec3b(1,1,1);

	cv::Mat out = test.getAppModel().imageToTexture(in);
	//cout << out << endl;

	double expT[15] = {1.0,2.0,3.0,7.0,3.0,1.0,2.0,3.0,7.0,3.0,1.0,2.0,3.0,7.0,3.0};
	cv::Mat expTex = cv::Mat(1, 15, CV_64F, expT);
	eq = eq & matEqualWithinTol(out, expTex, TOL);

	// test whether goes back to original as should
	cv::Mat im = test.getAppModel().textureToImage(expTex);
	// original was bytes, now need in doubles
	cv::Mat inD;
	in.convertTo(inD, CV_64FC3);
	//cout << im.at<cv::Vec3d>(0,1) << endl;
	//cout << im.at<cv::Vec3d>(0,2) << endl;
	//cout << im.at<cv::Vec3d>(1,2) << endl;
	//cout << im.at<cv::Vec3d>(2,2) << endl;
	//cout << im.at<cv::Vec3d>(2,0) << endl;
	eq = eq & matEqualWithinTol(im, inD, TOL);

	return eq;
}

void test(string path)
{
	bool eqLoad = testLoading(path);
	bool eqFit = testFitImage(path);
	bool eqP2T = testParams2Texture(path);
	bool eqEst = testAgeEst(path);
	bool eqSynth = testAgeSynth(path);
	bool eqImTexConv = testImageTextureConv(path);

	cout << endl << "Passed load test: " << eqLoad << endl;
	cout << "Passed fitting to appearance model test: " << eqFit << endl;
	cout << "Passed converting from appearance model parameters to texture test: " << eqP2T << endl;
	cout << "Passed age estimation test: " << eqEst << endl;
	cout << "Passed age synthesis test: " << eqSynth << endl;
	cout << "Passed image texture conversion test: " << eqImTexConv << endl;

	cout << "Press any key to continue" << endl;
	cin.ignore();
}

void main()
{
	//test("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test5");

	Model m = Model("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\image");
	cv::Mat texture = m.getField("texture");

	AgeingModel test = AgeingModel("C:\\dataset\\Models\\Model5");

	cv::Mat im = test.getAppModel().textureToImage(texture);
	cv::imshow("test", im/255);

	cv::waitKey();

	cv::Mat params = test.getAppModel().fitImageToAppModel(texture);
	//cout << params << endl;

	double age = test.getAgeEstModel().predictAge(params);

	cv::Mat older = test.changeFaceAge(params, 40, 'm');

	cv::Mat i = test.getAppModel().textureToImage(test.getAppModel().appParamsToTexture(params));
	cv::imshow("test", i/255);
	cv::Mat old = test.getAppModel().textureToImage(test.getAppModel().appParamsToTexture(older));
	cv::imshow("older", old/255);

	cv::waitKey();
}
