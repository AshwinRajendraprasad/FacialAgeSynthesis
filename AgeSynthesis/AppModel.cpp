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

cv::Mat AppModel::imageToTexture(cv::Mat image)
{
	cv::Mat mask = *getField(FieldNames::APP_MASK);

	if (mask.size != image.size)
		throw "Mask and image are different sizes";

	int numPixels = cv::sum(mask)[0];
	cv::Mat texture = cv::Mat(1, numPixels, CV_64F);

	int tCounter = 0;  //Counter to know which element in the texture to fill next
	// For each element in image, if the mask is a 1 then put it into the texture, else do nothing.
	for (int i=0; i<image.rows; i++)
	{
		for (int j=0; j<image.cols; j++)
		{
			if (mask.at<double>(i,j) == 1)
			{
				texture.at<double>(tCounter) = image.at<double>(i,j);
				tCounter++;
			}
		}
	}

	return texture;

}

cv::Mat AppModel::textureToImage(cv::Mat texture)
{
	cv::Mat mask = *getField(FieldNames::APP_MASK);

	// Create an image the same size as the mask
	cv::Mat image = cv::Mat::zeros(mask.rows, mask.cols, CV_64F);

	int tCounter = 0;
	// For each element in the image, if the mask is 1 then put the texture value in, else leave as 0
	for (int i=0; i<image.rows; i++)
	{
		for (int j=0; j<image.cols; j++)
		{
			if (mask.at<double>(i,j) == 1)
			{
				image.at<double>(i,j) = texture.at<double>(tCounter);
				tCounter++;
			}
		}
	}

	return image;
}
