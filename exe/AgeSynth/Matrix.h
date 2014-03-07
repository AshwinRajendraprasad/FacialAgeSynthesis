#pragma once

#include <vector>
#include <cv.h>

using namespace std;

class Matrix
{
public:
	Matrix(void);
	~Matrix(void);

	void insertRow(vector<double>* row);
	cv::Mat* convertToCVMat();

private:
	vector<vector<double>* >* m;
};

