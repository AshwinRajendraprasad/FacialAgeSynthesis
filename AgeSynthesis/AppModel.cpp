#include "AppModel.h"

AppModel::AppModel(void)
{
}

AppModel::AppModel(string path) : TransModel(path)
{
	cout << "Loading app.transform model" << endl;
	map<string, cv::Mat*> transMap = LoadSingleModel(path + "\\Transform");
	// Get the scale and offset from the map and put into the transform structure
	map<string, cv::Mat*>::const_iterator it = transMap.find(FieldNames::APP_SCALE);
	cv::Mat scale = *it->second;
	it = transMap.find(FieldNames::APP_OFFSET);
	cv::Mat offset = *it->second;

	setTransform(scale, offset);
}


AppModel::~AppModel(void)
{
}

double AppModel::getScale()
{
	// There is only one value in this, so return it
	return getTransform().getScale().at<double>(0,0);
}

double AppModel::getOffset()
{
	// There is only one value in this, so return it
	return getTransform().getOffset().at<double>(0,0);
}

AppModel::Transform AppModel::getTransform()
{
	return TransModel::getTransform();
}

cv::Mat AppModel::fitImageToAppModel(cv::Mat texture)
{
	// b = modes_inv * ((texture - offset)/scale - mean_texture)
	cv::Mat modes_inv = *getField(FieldNames::APP_MODES_INV);
	cv::Mat meanTexture = *getField(FieldNames::APP_MEAN_T);

	cv::Mat afterTrans = (texture - getOffset())/getScale() - meanTexture;

	cv::Mat appParams = modes_inv * afterTrans.t();

	return appParams;
}

cv::Mat AppModel::appParamsToTexture(cv::Mat appParams)
{
	// texture = (b*modes + mean_texture) * scale + offset
	cv::Mat modes = *getField(FieldNames::APP_MODES);
	cv::Mat meanTexture = *getField(FieldNames::APP_MEAN_T);

	cv::Mat texture = (meanTexture.t() + modes*appParams) * getScale() + getOffset();

	return texture;
}
