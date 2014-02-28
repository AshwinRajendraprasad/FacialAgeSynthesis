#include "Model.h"


Model::Model(string path)
{
	cout << "Loading basic model" << endl;
	// Load model.txt into fields
	fields = LoadSingleModel(path);
}

Model::Model(void)
{
}

Model::~Model(void)
{
}

map<string, cv::Mat* > Model::LoadSingleModel(std::string path) {

	string filename = path + "\\model.txt";
	ifstream infile(filename);

	string line;
	map<string, cv::Mat* > fields;
	Mat m = Mat();
	string name;
	while (getline(infile, line)) {
		stringstream ss(line);
		vector<double>* vec = new vector<double>();
		// If not digit then either space (split between fields), field name, comma or minus
		if (!isdigit(ss.peek()) && ss.peek() != ',' && ss.peek() != '-') {
			if (ss.peek() == ' ') {
				// Insert the field into the map
				cv::Mat* cvMat = m.convertToCVMat();
				//// Transpose the modes matrix as this is how it is used
				//if (name=="modes")
				//{
				//	cv::Mat tmp = cvMat->t();
				//	cvMat = &tmp;
				//}
				fields.insert(pair<string, cv::Mat*>(name, cvMat));
				// Need to clear the matrix and set the row counter back to 0
				m = Mat();
			} else {  // Must be a string, so its the name of the field
				name = line;
			}
			continue;
		}
		double val;
		while (ss >> val) {
			vec->push_back(val);

			if (ss.peek() == ',')
				ss.ignore();
		}
		// end of line, add to matrix
		m.insertRow(vec);
	}

	return fields;
}

cv::Mat* Model::getField(string name)
{
	map<string, cv::Mat*>::const_iterator it = fields.find(name);
	if (it != fields.end())
	{
		// Found element
		return it->second;
	}
	else
		// If not found then return NULL, the caller needs to check whether pointer is NULL
		return NULL;
}

