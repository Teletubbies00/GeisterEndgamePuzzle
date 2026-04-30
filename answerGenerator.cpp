#include<iostream>
#include"answerGenerator.h"



void solveFromCSV(const std::string& inputCsvPath, const std::string& outputCsvPath, const std::string& mode) {

    Data::PuzzlesLoader puzzles(inputCsvPath);
    std::ofstream outFile(outputCsvPath);
    if (!outFile.is_open()) {
        std::cerr << "無法建立輸出檔案: " << outputCsvPath << std::endl;
        return;
    }

    outFile << "Board,Solution\n";

    for (const auto& puzzle : puzzles.puzzleSet) {
        Search search(mode, "n");
        search.setNeedAns(true);
        search.setNeedReadableAns(true);
        search.onlyWantSteps = true;
        int actionNum = search.think(puzzle.board, puzzle.eBnum, puzzle.maxDepth);

        std::string solutionSteps = "no solution";
        if (actionNum > 0) {
            solutionSteps = search.answerBoard;

            // 移除換行符號，讓整筆 solution 變成單行
            std::replace(solutionSteps.begin(), solutionSteps.end(), '\n', ' ');

            // 去掉頭尾多餘空白
            auto start = solutionSteps.find_first_not_of(' ');
            auto end = solutionSteps.find_last_not_of(' ');
            if (start != std::string::npos)
                solutionSteps = solutionSteps.substr(start, end - start + 1);
        }

        // 用引號包住，防止內容裡的逗號破壞 CSV 格式
        outFile << "\"" << puzzle.board << "\","
            << "\"" << solutionSteps << "\"\n";

        std::cout << "Board: " << puzzle.board
            << " | steps: " << actionNum << "\n";
    }

    outFile.close();
    std::cout << "All answers are saved in: " << outputCsvPath << std::endl;
}




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
