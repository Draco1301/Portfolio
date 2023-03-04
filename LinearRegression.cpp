#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>

using namespace std::chrono;
using namespace std;

int dataSize = 0;
int testPoint = 800;
int iterations = 500;
double learning_rate = 0.01; //0.0001;

double sigmoid(double x) {
	return 1.0 / (1.0 + exp(-x));
}

void gradient_descent(vector<int> target, vector<int> parameter, vector<double>& returnvector) {
	double w0 = 0.5;
	double w1 = 0.5;

	double sum_w0 = 0;
	double sum_w1 = 0;

	double lowest_cost = 1;
	double lowest_w0 = w0;
	double lowest_w1 = w1;

	for (int i = 0; i < iterations; i++) {
		double cost = 0;
		for (int d = 0; d < testPoint; d++) {
			double P = sigmoid(w0 + w1 * parameter[d]);
			sum_w0 += (P - target[d]);
			sum_w1 += (P - target[d]) * parameter[d];
			cost += -target[d] * log(P) - (1 - target[d]) * log(1 - P); // logistic regression cost function
		}
		cost /= 800;

		if (cost < lowest_cost) {
			lowest_cost = cost;
			lowest_w0 = w0;
			lowest_w1 = w1;
			//cout << i << ": " << cost << "\tW0: " << w0 << "\tW1:" << w1 << endl;
		}

		w0 -= sum_w0 / testPoint * learning_rate;
		w1 -= sum_w1 / testPoint * learning_rate;
	}

	returnvector[0] = lowest_w0;
	returnvector[1] = lowest_w1;
}

double accuracy(vector<int> target, vector<int> parameter, vector<double> coeffecients) {
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		int r = sigmoid(coeffecients[0] + coeffecients[1] * parameter[i]) > 0.5 ? 1: 0;
		if (r == target[i]) {
			counter++;
		}
	}
	return counter/(dataSize - testPoint);
}

double sensitivity(vector<int> target, vector<int> parameter, vector<double> coeffecients) {
	double TP_counter = 0;
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		if (1 != target[i]) {
			continue;;
		}
		counter++;

		int r = sigmoid(coeffecients[0] + coeffecients[1] * parameter[i]) > 0.5 ? 1 : 0;
		if (r == target[i]) {
			TP_counter++;
		}
	}
	return TP_counter / counter;
}

double specificiety(vector<int> target, vector<int> parameter, vector<double> coeffecients) {
	double TN_counter = 0;
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		if (0 != target[i]) {
			continue;;
		}
		counter++;

		int r = sigmoid(coeffecients[0] + coeffecients[1] * parameter[i]) > 0.5 ? 1 : 0;
		if (r == target[i]) {
			TN_counter++;
		}
	}
	return TN_counter / counter;
}

int main() {
	//data
	vector<int> pclass(1046);
	vector<int> survived(1046);
	vector<int> sex(1046);
	vector<int> age(1046);

	//read file
	ifstream inFS;
	string line;
	string pclass_in, survived_in, sex_in, age_in;

	inFS.open("titanic_project.csv");
	if (!inFS.is_open()) {
		cout << "File not found" << endl;
		return 1;
	}

	std::getline(inFS, line);
	while (inFS.good()) {
		std::getline(inFS, pclass_in, ',');
		std::getline(inFS, pclass_in, ',');
		std::getline(inFS, survived_in, ',');
		std::getline(inFS, sex_in, ',');
		std::getline(inFS, age_in, '\n');

		pclass.at(dataSize) = stoi(pclass_in);
		survived.at(dataSize) = stoi(survived_in);
		sex.at(dataSize) = stoi(sex_in);
		age.at(dataSize) = stoi(age_in);

		dataSize++;
	}


	vector<double> coeffecients(2);
	auto start = high_resolution_clock::now();
	gradient_descent(survived, sex, coeffecients);
	auto stop = high_resolution_clock::now();

	cout << "Logistic Regression\t\t\t\tTime in millisec" << endl;
	cout << "Coefficients W0 + W1 * X" << endl;
	cout << coeffecients[0] << (coeffecients[1] > 0 ? "+" : " ") << coeffecients[1] << "X\t\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000 << "s (" << iterations << " Iterations)" << endl;
	cout << endl;
	cout << "Chances of survival" << endl;
	cout << "female " << sigmoid(coeffecients[0]) << endl;
	cout << "male   " << sigmoid(coeffecients[0] + coeffecients[1]) << endl;
	cout << endl;

	start = high_resolution_clock::now();
	cout << "Accuracy:\t" << accuracy(survived, sex, coeffecients);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;

	start = high_resolution_clock::now();
	cout << "Sensitivity:\t" << sensitivity(survived, sex, coeffecients);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;

	start = high_resolution_clock::now();
	cout << "Specificity:\t" << specificiety(survived, sex, coeffecients);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;


}