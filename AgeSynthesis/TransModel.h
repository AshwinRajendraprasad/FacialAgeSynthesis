#pragma once
#include "model.h"

using namespace std;

class TransModel :
	public Model
{
public:
	TransModel(void);
	TransModel(string path);
	~TransModel(void);

	struct Transform
	{
	public:
		Transform(void);
		Transform(cv::Mat s, cv::Mat o);

		cv::Mat getScale();
		cv::Mat getOffset();
		
	private:
		cv::Mat scale;
		cv::Mat offset;
	};

	virtual Transform getTransform();

protected:
	void setTransform(cv::Mat s, cv::Mat o);

private:
	Transform transform;
};

