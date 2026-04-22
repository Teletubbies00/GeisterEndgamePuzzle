#include<iostream>
#include"answerGenerator.h"
using namespace std;


void SolveFromCSV(const string& inputCsvPath, const string& outputCsvPath, const string& mode) {
    ifstream inFile(inputCsvPath);
    if (!inFile.is_open()) {
        cerr << "無法開啟輸入檔案: " << inputCsvPath << endl;
        return;
    }

    ofstream outFile(outputCsvPath);
    if (!outFile.is_open()) {
        cerr << "無法建立輸出檔案: " << outputCsvPath << endl;
        return;
    }

    //outFile << "Board,eBnum,MaxDepth,ActionNum,CalCount,MaxPn,MaxDn,Solution\n";
    outFile << "Board,Solution\n";


    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string boardStr, eBnumStr, maxDepthStr;

        // CSV 格式：盤面,eBnum,maxDepth
        getline(ss, boardStr, ',');
        getline(ss, eBnumStr, ',');
        getline(ss, maxDepthStr, ',');

        int eBnum = stoi(eBnumStr);
        int maxDepth = stoi(maxDepthStr);

        // 呼叫你的 Search 引擎
        Search search(mode, "n");
        search.setNeedAns(true);

        int actionNum = search.think(boardStr, eBnum, maxDepth);

        unsigned int calCount = search.returnCount();
        auto maxpn = search.returnMaxPn();
        auto maxdn = search.returnMaxDn();

        string solutionSteps = "no solution";
        if (actionNum > 0) {
            solutionSteps = search.answerBoard;
        }

        // 寫入 CSV，解答字串用雙引號包起來
        //outFile << boardStr << ","
        //    << eBnum << ","
        //    << maxDepth << ","
        //    << actionNum << ","
        //    << calCount << ","
        //    << maxpn << ","
        //    << maxdn << ","
        //    << "\"" << solutionSteps << "\"\n";
        outFile << boardStr << ","
            << "\"" << solutionSteps << "\"\n";
        cout << solutionSteps << endl;
        cout << "Board: " << boardStr << " | steps: " << actionNum << "\n";
    }

    inFile.close();
    outFile.close();
    cout << "All answers are saved in : " << outputCsvPath << endl;
}
//
//
//// 這是這個新檔案專屬的主程式
//int main(int argc, char* argv[]) {
//    // 預設參數，你可以改成讀取命令列參數
//    string inputCsv = "questions.csv";  // 你的題目檔名
//    string outputCsv = "answers.csv";   // 你想要的輸出檔名
//    string mode = "n";                  // normal 模式
//
//    if (argc == 4) {
//        inputCsv = argv[1];
//        outputCsv = argv[2];
//        mode = argv[3];
//    }
//
//    cout << "--- 啟動獨立 CSV 解題工具 ---" << endl;
//    SolveFromCSV(inputCsv, outputCsv, mode);
//
//    return 0;
//}
