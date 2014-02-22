#pragma once

#include <vector>

using namespace std;

class Mat
{
public:
	Mat(void);
	~Mat(void);

	void insertRow(vector<double>* row);
	void clear();

private:
	vector<vector<double>* >* m;
};

