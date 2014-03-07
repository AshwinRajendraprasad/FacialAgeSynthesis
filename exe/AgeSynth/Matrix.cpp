#include "Matrix.h"


Matrix::Matrix(void)
{
	m = new vector<vector<double>* >();
}


Matrix::~Matrix(void)
{
}

void Matrix::insertRow(vector<double>* row)
{
	m->push_back(row);
}


cv::Mat* Matrix::convertToCVMat()
{
	int rows = m->size();
	// Assumes that all rows are same number of columns, which needs to be the case (and should be)
	int cols = (*m)[0]->size();

	cv::Mat* cvMat = new cv::Mat(rows, cols, CV_64F);
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<cols; j++)
		{
			// Just copies the value from (i,j) in matrix to (i,j) in opencv matrix
			cvMat->at<double>(i, j) = (*(*m)[i])[j];
		}
	}

	return cvMat;
}
