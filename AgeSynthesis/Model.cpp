#include "Model.h"


Model::Model(void)
{
}


Model::~Model(void)
{
}

void Model::LoadModel(std::string path) {
	LoadSingleModel(path);
}

void Model::LoadSingleModel(std::string path) {
	using namespace std;

	string filename = path + "\\model.txt";
	ifstream infile(filename);

	string line;
	vector<Field> fields;
	Field f;
	while (getline(infile, line)) {
		stringstream ss(line);
		vector<float> vec;
		// If not digit then either space (split between fields), field name or comma
		if (!isdigit(ss.peek()) && ss.peek() != ',') {
			if (ss.peek() == ' ') {
				fields.push_back(f);
				f = Field();
			} else {
				f.name = line;
			}
			continue;
		}
		float i;
		while (ss >> i) {
			vec.push_back(i);

			if (ss.peek() == ',')
				ss.ignore();
		}
		// end of line, add to matrix
		cv::Mat temp = cv::Mat(vec, true);
		f.value.push_back(temp.t());
	}
}

void main() {
	Model model;
	model.LoadModel("C:\\Users\\Rowan\\Documents\\Cambridge Files\\Part II project\\Model\\test\\e");
}
