#pragma once
#include "Search.h"

void HelpMessageGetSol(const std::string& prog_name) {
    std::cout << "Usage: " << prog_name << " GetSol [board std::string] [#b] [(max #action)] [(RedWall)]" << std::endl;
#if OUTPUT_JP_MESSAGE
    std::cout << "- board std::string: 1x36 盤面文字列" << std::endl;
    std::cout << "- #b: 整数，敵青駒の数" << std::endl;
    std::cout << "- max #action: 最大探索深さ (選択的)" << std::endl;
    std::cout << "- RedWall: 赤駒壁利用なのかを確認（選択的）" << std::endl;
    std::cout << "例：" << prog_name << " GetSol ..........u.B.................Ru.... 1 7 RedWall" << std::endl;
#else
    std::cout << "- board std::string: 1x36 std::string showing the board" << std::endl;
    std::cout << "- #b: integer, the number of the opponent's blue pieces" << std::endl;
    std::cout << "- max #action: the depth limit of search (optional)" << std::endl;
    std::cout << "- RedWall: whether to check red-wall (optional)" << std::endl;
    std::cout << "Example: " << prog_name << " GetSol ..........u.B.................Ru.... 1 7 RedWall" << std::endl;
#endif
}

void GetSolution(const std::vector<std::string>& argvVec) {
    if ((argvVec.size() < 3) || (argvVec[1].length() != 36)) {
        HelpMessageGetSol(argvVec[0]);
        return;
    }

    std::string board = argvVec[1];
    bool is_partly_revealed = ((board.find("r") != std::string::npos)
        || (board.find("b") != std::string::npos));
    std::string mode = (is_partly_revealed ? std::string("p") : std::string("n"));
    int eB = stoi(argvVec[2]);
    int maxAction = 51;
    if ((argvVec.size() >= 4)
        && (argvVec[3].find_first_of("0123456789") != std::string::npos))
    {
        maxAction = stoi(argvVec[3]);
    }
    std::string subMode = (is_partly_revealed ? std::string("a") : std::string("n"));

    if (is_partly_revealed) {
#if OUTPUT_JP_MESSAGE
        std::cout << "一部公開" << std::endl;
#else
        std::cout << "partly-revealed" << std::endl;
#endif
    } else {
#if OUTPUT_JP_MESSAGE
        std::cout << "非公開" << std::endl;
#else
        std::cout << "not-revealed" << std::endl;
#endif
    }

    Search search(mode, subMode);
    search.setNeedAns(true);
    search.setNeedReadableAns(true);
    bool needPnDn = false;
    search.setNeedPnDn(needPnDn);
    int actionNum = search.think(board, eB, maxAction);
    if (actionNum == 0) { return; }

#if OUTPUT_JP_MESSAGE
    std::cout << search.returnCount() << " ノード数" << std::endl;
    std::cout << search.returnLastCount() << " 必勝手数ノード数" << std::endl;
#else
    std::cout << search.returnCount() << " nodes" << std::endl;
    std::cout << search.returnLastCount() << " nodes of #winning move" << std::endl;
#endif

#if OUTPUT_JP_MESSAGE
    std::cout << actionNum << " 手詰" << std::endl;
#else
    std::cout << actionNum << " moves" << std::endl;
#endif

    if (mode == std::string("p")) {
        Search subSearchBlueCapture(mode, "a");
        int actionNumBlueCapture = subSearchBlueCapture.think(board, eB, actionNum, actionNum - 1);
        if (actionNumBlueCapture == 0) {
#if OUTPUT_JP_MESSAGE
            std::cout << "<青駒全取り>" << std::endl;
#else
            std::cout << "<Capture-Win>" << std::endl;
#endif
        } else {
            search.think(board, eB, maxAction, actionNum - 1);
        }
    }
    bool needCheckRedWall = false;
    for (size_t i = 3; i < argvVec.size(); i++) {
        if (argvVec[i] == std::string("RedWall")) { needCheckRedWall = true; }
    }
    if (needCheckRedWall) {
        Search subSearchRedWall(mode, "r");
        int actionNumRedWall = subSearchRedWall.think(board, eB, actionNum);
        if (actionNumRedWall == 0) {
#if OUTPUT_JP_MESSAGE
            std::cout << "<赤駒壁利用>" << std::endl;
#else
            std::cout << "<Red-Wall>" << std::endl;
#endif
        }
    }
#if OUTPUT_JP_MESSAGE
    std::cout << "解答例" << std::endl;
#else
    std::cout << "Example Solution" << std::endl;
#endif
    std::cout << search.answerBoard << std::endl;
}

//
//inline void SolveFromCSV(const std::string& inputCsvPath, const std::string& outputCsvPath, const std::string& mode) {
//    std::ifstream inFile(inputCsvPath);
//    if (!inFile.is_open()) {
//        cerr << "無法開啟輸入檔案: " << inputCsvPath << std::endl;
//        return;
//    }
//
//    std::ofstream outFile(outputCsvPath);
//    if (!outFile.is_open()) {
//        cerr << "無法建立輸出檔案: " << outputCsvPath << std::endl;
//        return;
//    }
//
//    //outFile << "Board,eBnum,MaxDepth,ActionNum,CalCount,MaxPn,MaxDn,Solution\n";
//    outFile << "Board,Solution\n";
//
//
//    std::string line;
//    while (getline(inFile, line)) {
//        if (line.empty()) continue;
//
//        std::stringstream ss(line);
//        std::string boardStr, eBnumStr, maxDepthStr;
//
//        // CSV 格式：盤面,eBnum,maxDepth
//        getline(ss, boardStr, ',');
//        getline(ss, eBnumStr, ',');
//        getline(ss, maxDepthStr, ',');
//
//        int eBnum = stoi(eBnumStr);
//        int maxDepth = stoi(maxDepthStr);
//
//        // 呼叫你的 Search 引擎
//        Search search(mode, "n");
//        search.setNeedAns(true);
//        search.setNeedReadableAns(true);
//
//        int actionNum = search.think(boardStr, eBnum, maxDepth);
//
//        unsigned int calCount = search.returnCount();
//        auto maxpn = search.returnMaxPn();
//        auto maxdn = search.returnMaxDn();
//
//        std::string solutionSteps = "無解或超過深度";
//        if (actionNum > 0) {
//            solutionSteps = search.answerBoard;
//        }
//
//        // 寫入 CSV，解答字串用雙引號包起來
//        //outFile << boardStr << ","
//        //    << eBnum << ","
//        //    << maxDepth << ","
//        //    << actionNum << ","
//        //    << calCount << ","
//        //    << maxpn << ","
//        //    << maxdn << ","
//        //    << "\"" << solutionSteps << "\"\n";
//        outFile << boardStr << ","
//            << "\"" << solutionSteps << "\"\n";
//        std::cout << solutionSteps << std::endl;
//        std::cout << "已處理盤面: " << boardStr << " | 步數: " << actionNum << "\n";
//    }
//
//    inFile.close();
//    outFile.close();
//    std::cout << "所有題目解題完畢，解答已存至: " << outputCsvPath << std::endl;
//}
//
