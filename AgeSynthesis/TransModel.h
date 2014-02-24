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
		Transform(double s, double o);

		double getScale();
		double getOffset();
		
	private:
		double scale;
		double offset;
	};

	Transform getTransform();

private:
	Transform transform;
};

