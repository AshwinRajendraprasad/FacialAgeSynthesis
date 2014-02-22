#include "Model.h"


Model::Model(void)
{
}


Model::~Model(void)
{
}

map<string, Mat > Model::LoadSingleModel(std::string path) {

	string filename = path + "\\model.txt";
	ifstream infile(filename);

	string line;
	map<string, Mat > fields;
	Mat m = Mat();
	string name;
	while (getline(infile, line)) {
		stringstream ss(line);
		vector<double>* vec = new vector<double>();
		// If not digit then either space (split between fields), field name, comma or minus
		if (!isdigit(ss.peek()) && ss.peek() != ',' && ss.peek() != '-') {
			if (ss.peek() == ' ') {
				// Insert the field into the map
				fields.insert(pair<string, Mat>(name, m));
				// Need to clear the matrix and set the row counter back to 0
				m = Mat();
			} else {
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

void main() {
	Model model;
	model.LoadSingleModel("C:\\dataset\\Models\\Model1\\AgeEst");
}
