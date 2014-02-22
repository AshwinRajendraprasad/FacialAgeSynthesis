#include "Mat.h"


Mat::Mat(void)
{
	m = new vector<vector<double>* >();
}


Mat::~Mat(void)
{
}

void Mat::insertRow(vector<double>* row)
{
	m->push_back(row);
}

void Mat::clear()
{
	m->clear();
}
