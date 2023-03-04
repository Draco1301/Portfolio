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

double Gaussian(double x, double avg, double std) {
	return pow(2.71828182846, -(x - avg) * (x - avg) / (2 * std * std)) / (2.50662827463 * std);
}

void naiveBayes(vector<int> survived, vector<int> sex, vector<int> pclass, vector<int> age, vector<double>& probabilityReturn, vector<double>& ageReturn) {

	//P(Y) * testPoint
	double survived_counter[2] = { 0,0 };

	//P(X & Y) * testPoint
	int sex_counter[2][2] = { {0,0}, {0,0} }; //lol
	int pclass_counter[2][3] = { {0,0,0}, {0,0,0} };

	double ageStat[2][2] = { {0, 0} , { 0, 0 } };

	//count
	for (int i = 0; i < testPoint; i++) {

		survived_counter[survived[i]]++;

		sex_counter[survived[i]][sex[i]]++;
		pclass_counter[survived[i]][pclass[i] - 1]++;

		ageStat[survived[i]][0] += age[i];
	}

	//P(Y)
	survived_counter[0] /= testPoint;
	survived_counter[1] /= testPoint;

	//average
	ageStat[0][0] /= testPoint;
	ageStat[1][0] /= testPoint;

	//standard deviation
	for (int i = 0; i < testPoint; i++) {
		ageStat[survived[i]][1] += (ageStat[survived[i]][0] - age[i]) * (ageStat[survived[i]][0] - age[i]);
	}
	ageStat[0][1] = sqrt(ageStat[0][1] / testPoint);
	ageStat[1][1] = sqrt(ageStat[1][1] / testPoint);

	//P(X | Y)
	double sexCorrelation[2][2] = { {static_cast<double>(sex_counter[0][0]) / (sex_counter[0][0] + sex_counter[0][1]), static_cast<double>(sex_counter[0][1]) / (sex_counter[0][0] + sex_counter[0][1])},
									{static_cast<double>(sex_counter[1][0]) / (sex_counter[1][0] + sex_counter[1][1]), static_cast<double>(sex_counter[1][1]) / (sex_counter[1][0] + sex_counter[1][1])} };
	double pclassCorrelation[2][3] = { { static_cast<double>(pclass_counter[0][0]) / (pclass_counter[0][0] + pclass_counter[0][1] + pclass_counter[0][2]), static_cast<double>(pclass_counter[0][1]) / (pclass_counter[0][0] + pclass_counter[0][1] + pclass_counter[0][2]), static_cast<double>(pclass_counter[0][2]) / (pclass_counter[0][0] + pclass_counter[0][1] + pclass_counter[0][2]) },
									   { static_cast<double>(pclass_counter[1][0]) / (pclass_counter[1][0] + pclass_counter[1][1] + pclass_counter[1][2]), static_cast<double>(pclass_counter[1][1]) / (pclass_counter[1][0] + pclass_counter[1][1] + pclass_counter[1][2]), static_cast<double>(pclass_counter[1][2]) / (pclass_counter[1][0] + pclass_counter[1][1] + pclass_counter[1][2]) } };

	//P(Y | X)
	double probability[2][2][3] = { { { 0, 0, 0 },
										{ 0, 0, 0 } },
									{ { 0, 0, 0 },
									   { 0, 0, 0 } } };

	for (int Y = 0; Y < 2; Y++) {
		for (int s = 0; s < 2; s++) {
			for (int c = 0; c < 3; c++) {
				//P(Y | X) = P(Y) * P(sex | Y) * P(pclass | Y)
				probability[Y][s][c] = survived_counter[Y] * sexCorrelation[Y][s] * pclassCorrelation[Y][c];
				//cout << "P(Y):" << survived_counter[Y] << " P(s|Y):" << sexCorrelation[Y][s] << " P(c|Y):" << pclassCorrelation[Y][c] << "\t";
				//cout << "P(Y:" << Y<< " | s:" << s << ", c:" << c+1 << ")" << probability[Y][s][c] << endl;
			}
		}
	}

	probabilityReturn[0] = probability[0][0][0];
	probabilityReturn[1] = probability[0][0][1];
	probabilityReturn[2] = probability[0][0][2];
	probabilityReturn[3] = probability[0][1][0];
	probabilityReturn[4] = probability[0][1][1];
	probabilityReturn[5] = probability[0][1][2];
	probabilityReturn[6] = probability[1][0][0];
	probabilityReturn[7] = probability[1][0][1];
	probabilityReturn[8] = probability[1][1][2];
	probabilityReturn[9] = probability[1][1][0];
	probabilityReturn[10] = probability[1][1][1];
	probabilityReturn[11] = probability[1][1][2];

	ageReturn[0] = ageStat[0][0];
	ageReturn[1] = ageStat[0][1];
	ageReturn[2] = ageStat[1][0];
	ageReturn[3] = ageStat[1][1];

}

double accuracy(vector<int> target, vector<int> sex, vector<int> pclass, vector<int> age, double probabilityReturn[2][2][3], double ageReturn[2][2]) {
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		int r = Gaussian(age[i], ageReturn[1][0], ageReturn[1][1]) * probabilityReturn[1][sex[i]][pclass[i] - 1] > Gaussian(age[i], ageReturn[0][0], ageReturn[0][1]) * probabilityReturn[0][sex[i]][pclass[i] - 1] ? 1 : 0;
		if (r == target[i]) {
			counter++;
		}
	}
	return counter / (dataSize - testPoint);
}

double sensitivity(vector<int> target, vector<int> sex, vector<int> pclass, vector<int> age, double probabilityReturn[2][2][3], double ageReturn[2][2]) {
	double TP_counter = 0;
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		if (1 != target[i]) {
			continue;;
		}
		counter++;

		int r = Gaussian(age[i], ageReturn[1][0], ageReturn[1][1]) * probabilityReturn[1][sex[i]][pclass[i] - 1] > Gaussian(age[i], ageReturn[0][0], ageReturn[0][1]) * probabilityReturn[0][sex[i]][pclass[i] - 1] ? 1 : 0;

		if (r == target[i]) {
			TP_counter++;
		}
	}
	return TP_counter / counter;
}

double specificiety(vector<int> target, vector<int> sex, vector<int> pclass, vector<int> age, double probabilityReturn[2][2][3], double ageReturn[2][2]) {
	double TN_counter = 0;
	double counter = 0;
	for (int i = testPoint; i < dataSize; i++) {
		if (0 != target[i]) {
			continue;;
		}
		counter++;

		int r = Gaussian(age[i], ageReturn[1][0], ageReturn[1][1]) * probabilityReturn[1][sex[i]][pclass[i] - 1] > Gaussian(age[i], ageReturn[0][0], ageReturn[0][1]) * probabilityReturn[0][sex[i]][pclass[i] - 1] ? 1 : 0;
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

	vector<double> probVec(12);
	vector<double> probAge(4);

	double probs[2][2][3];
	double ageStats[2][2];

	auto start = high_resolution_clock::now();
	naiveBayes(survived, sex, pclass, age, probVec, probAge);
	auto stop = high_resolution_clock::now();
	cout << "\t\t\t\t\t\tTime in millisec" << endl;
	cout << "Naive Bayes\t\t\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;
	cout << endl;

	probs[0][0][0] = probVec[0];
	probs[0][0][1] = probVec[1];
	probs[0][0][2] = probVec[2];
	probs[0][1][0] = probVec[3];
	probs[0][1][1] = probVec[4];
	probs[0][1][2] = probVec[5];
	probs[1][0][0] = probVec[6];
	probs[1][0][1] = probVec[7];
	probs[1][0][2] = probVec[8];
	probs[1][1][0] = probVec[9];
	probs[1][1][1] = probVec[10];
	probs[1][1][2] = probVec[11];

	ageStats[0][0] = probAge[0];
	ageStats[0][1] = probAge[1];
	ageStats[1][0] = probAge[2];
	ageStats[1][1] = probAge[3];

	cout << "\t\tSurvived\tDied" << endl;
	cout << "Male" << endl;
	cout << "Pclass 1\t" << probs[1][1][0] << "\t" << probs[0][1][0] << endl;
	cout << "Pclass 2\t" << probs[1][1][1] << "\t" << probs[0][1][1] << endl;
	cout << "Pclass 3\t" << probs[1][1][2] << "\t" << probs[0][1][2] << endl;
	cout << "Female" << endl;
	cout << "Pclass 1\t" << probs[1][0][0] << "\t" << probs[0][0][0] << endl;
	cout << "Pclass 2\t" << probs[1][0][1] << "\t" << probs[0][0][1] << endl;
	cout << "Pclass 3\t" << probs[1][0][2] << "\t" << probs[0][0][2] << endl;
	cout << "Age" << endl;
	cout << "Avg\t\t" << ageStats[1][0] << "\t\t" << ageStats[0][0] << endl;
	cout << "Std\t\t" << ageStats[1][1] << "\t\t" << ageStats[0][1] << endl;
	cout << endl;

	start = high_resolution_clock::now();
	cout << "Accuracy:\t" << accuracy(survived, sex, pclass, age, probs, ageStats);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;

	start = high_resolution_clock::now();
	cout << "Sensitivity:\t" << sensitivity(survived, sex, pclass, age, probs, ageStats);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;

	start = high_resolution_clock::now();
	cout << "Specificity:\t" << specificiety(survived, sex, pclass, age, probs, ageStats);
	stop = high_resolution_clock::now();
	cout << "\t\t\t" << duration_cast<microseconds>(stop - start).count() / 1000.0 << "s" << endl;


}