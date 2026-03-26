#include<string>
#include<iostream>
#include <fstream>
#include <sstream>
#include"CsvToJson.h"
using namespace std;

// viewer_and_generated_puzzles\allprob
namespace Data {
	// 處理csv檔內容為題目、解法(解法可能包含題目還沒決定)
	void convertCsvToJson(const string& csvPath, const string& jsonPath) {
		ifstream file(csvPath);
		if (!file.is_open()) {
			cerr << "csvPath is not exist";
			return;
		}

		string puzzle;
		ordered_json allPuzzles; // question 可以在answer前面 看的舒服

		while (getline(file, puzzle)) {
			if (puzzle == "") {
				continue; 
			}
			string question, ansStep;
			stringstream ss(puzzle);
			getline(ss, question, ',');

			ordered_json onePuzzle;
			vector<string> answer;
			//拆解答案
			while (getline(ss, ansStep, ',')) {
				answer.push_back(ansStep);
			}

			onePuzzle["question"] = question;
			onePuzzle["answer"] = answer;
			allPuzzles.push_back(onePuzzle);
		}

		ofstream outFile(jsonPath);

		if (outFile.is_open()) {
			outFile << allPuzzles.dump(4);
			outFile.close();
			cout << "Success! Json file path : " << jsonPath << endl;
		}
		else {
			cerr << "Error: Can't create json file" << endl;
		}
	}
}


/* test
int main() {
	string csvPath = "test.csv";
	string jsonPath = "result.json";
	Data::convertCsvToJson(csvPath, jsonPath);
	return 0;
}
*/

