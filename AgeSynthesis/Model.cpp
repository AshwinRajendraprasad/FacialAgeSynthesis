#include "Model.h"

#include <iostream>
#include <bitset>

Model::Model(string path)
{
	cout << "Loading basic model" << endl;
	// Load model.bin into fields
	fields = LoadSingleModelB(path);
}

Model::Model(void)
{
}

Model::~Model(void)
{
}

// DEPRECATED - use binary for speed
map<string, cv::Mat > Model::LoadSingleModel(std::string path)
{

	string filename = path + "\\model.txt";
	ifstream infile(filename);

	string line;
	map<string, cv::Mat > fields;
	Mat m = Mat();
	string name;
	while (getline(infile, line)) {
		stringstream ss(line);
		vector<double>* vec = new vector<double>();
		// If not digit then either space (split between fields), field name, comma or minus
		if (!isdigit(ss.peek()) && ss.peek() != ',' && ss.peek() != '-') {
			if (ss.peek() == ' ') {
				// Insert the field into the map
				cv::Mat cvMat = *m.convertToCVMat();
				//// Transpose the modes matrix as this is how it is used
				//if (name=="modes")
				//{
				//	cv::Mat tmp = cvMat->t();
				//	cvMat = &tmp;
				//}
				fields.insert(pair<string, cv::Mat>(name, cvMat));
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

map<string, cv::Mat > Model::LoadSingleModelB(std::string path)
{

	string filename = path + "\\model.bin";
	// Open the file for reading, in binary format and at the end (to find the size)
	ifstream file(filename, ios::in | ios::binary | ios::ate);

	streampos size;
	char * memblock;

	map<string, cv::Mat > fields;

	// Load the file into memory
	if (file.is_open())
	{
		size = file.tellg();  // opened at the end so this gives the size of the file in bytes
		memblock = new char [size];
		// Seek back to the beginning and read the whole file in
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();

		string name;
		
		int i=0;
		// keep going until have done all of the data
		do
		{
			char buf[50];  //names shouldn't be larger than 50 chars
			int nameCounter=0;
			// up to the first 0 char is the field name
			for (; i<size; i++)
			{
				if (memblock[i] == 0)
				{
					// Only want the actual string, not the rest of it
					name = string(buf, buf+nameCounter);
					break;
				}
				buf[nameCounter++] = memblock[i];
			}
			// Now need to read the 2 dimensions
			buf[4];  // to fit an int
			for (int intCounter1=0; intCounter1<4; intCounter1++)
				buf[intCounter1] = memblock[++i];
			int rows = bytes2int(buf);
			buf[4];
			for (int intCounter2=0; intCounter2<4; intCounter2++)
				buf[intCounter2] = memblock[++i];
			int cols = bytes2int(buf);

			// Load the data for this field
			cout << "Loading " << name << endl;
			cv::Mat mat(rows, cols, CV_64F);
			// data is written in column order
			for (int y=0; y<cols; y++)
			{
				for (int x=0; x<rows; x++)
				{
					buf[8];  // to fit one double in
					// load in the next double
					for (int doubCounter=0; doubCounter<8; doubCounter++)
						buf[doubCounter] = memblock[++i];
					mat.at<double>(x,y) = bytes2double(buf);
				}
			}
			i++; //necessary to get counter to correct place
			fields.insert(pair<string, cv::Mat>(name, mat));
		} while (i<size);
		
		delete[] memblock;
	}

	return fields;
}

cv::Mat Model::getField(string name)
{
	map<string, cv::Mat>::const_iterator it = fields.find(name);
	if (it != fields.end())
	{
		// Found element
		return it->second;
	}
	else
		// If not found then return an empty Mat, the caller needs to check whether it is empty
		return cv::Mat();
}


int Model::bytes2int(char* bytes)
{
	int tmp;
	memcpy(&tmp, bytes, sizeof(int));
	return tmp;
}

double Model::bytes2double(char* bytes)
{
	double tmp;
	memcpy(&tmp, bytes, sizeof(double));
	return tmp;
}