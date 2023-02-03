#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

int dataSize = 0;
double sum(vector<double> vect) {
	double sum = 0;
	for (auto i = vect.begin(); i != vect.end(); i++) {
		sum += *i;
	}
	return sum;
}

double mean(vector<double> vect) {
	return sum(vect) / dataSize;
}

double median(vector<double> vect) {

	vector<double> temp;
	for (int i = 0; i < vect.size(); i++) {
		temp.push_back(vect[i]);
	}
	
	sort(temp.begin(), temp.end());
	
	if (dataSize %2 ==0) {
		return (temp.at(dataSize / 2) + temp.at(dataSize / 2 + 1)) / 2;
	} else {
		return temp.at(dataSize / 2);
	}
}

double range(vector<double> vect) { 
	vector<double> temp;
	for (int i = 0; i < vect.size(); i++) {
		temp.push_back(vect[i]);
	}

	sort(temp.begin(), temp.end());

	return temp.at(dataSize -1) - temp.at(0);
}

double covariance(vector<double> vect1, vector<double> vect2) {
	double mean1 = mean(vect1);
	double mean2 = mean(vect2);

	double sum = 0;
	auto n = vect2.begin();
	for (auto i = vect1.begin(); i != vect1.end(); i++, n++) {
		sum += (*i - mean1) * (*n - mean2);
	}
	return sum / (dataSize - 1.0 );
}

double stdDeviation(vector<double> vect) {
	double sum = 0;
	double vectMean = mean(vect);

	for (auto i = vect.begin(); i != vect.end(); i++) {
		sum += (*i - vectMean) * (*i - vectMean);
	}

	return sqrt(sum / dataSize);
}

double correlation(vector<double> vect1, vector<double> vect2) {
	return covariance(vect1, vect2) / (stdDeviation(vect1) * stdDeviation(vect2));
}

int main() {

	//data
	vector<double> rm(1000);
	vector<double> medv(1000);

	//read file
	ifstream inFS;
	string line;
	string rm_in, medv_in;

	inFS.open("Boston.csv");
	if (!inFS.is_open()) {
		cout << "File not found" << endl;
		return 1;
	}

	std::getline(inFS, line);
	while (inFS.good()) {

		std::getline(inFS, rm_in, ',');
		std::getline(inFS, medv_in, '\n');

		rm.at(dataSize) = stof(rm_in);
		medv.at(dataSize) = stof(medv_in);

		dataSize++;
	}
	rm.resize(dataSize);
	medv.resize(dataSize);
				
	cout << "\trm\tmdev" << endl;
	cout << "sum\t" << sum(rm) << "\t" << sum(medv) << endl;
	cout << "mean\t" << mean(rm) << "\t" << mean(medv) << endl;
	cout << "median\t" << median(rm) << "\t" << median(medv) << endl;
	cout << "range\t" << range(rm) << "\t" << range(medv) << endl;
	cout << "covariance\t" << covariance(rm, medv) << endl;
	cout << "correlation\t" << correlation(rm, medv) << endl;

}

