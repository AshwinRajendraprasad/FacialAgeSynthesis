#pragma once

#include <vector>
#include <cv.h>

using namespace std;

class Mat
{
public:
	Mat(void);
	~Mat(void);

	void insertRow(vector<double>* row);
	cv::Mat* convertToCVMat();

private:
	vector<vector<double>* >* m;
};

