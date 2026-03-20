#pragma once
#include "Search.h"

void HelpMessageGetSol(const string& prog_name) {
    cout << "Usage: " << prog_name << " GetSol [board string] [#b] [(max #action)] [(RedWall)]" << endl;
#if OUTPUT_JP_MESSAGE
    cout << "- board string: 1x36 盤面文字列" << endl;
    cout << "- #b: 整数，敵青駒の数" << endl;
    cout << "- max #action: 最大探索深さ (選択的)" << endl;
    cout << "- RedWall: 赤駒壁利用なのかを確認（選択的）" << endl;
    cout << "例：" << prog_name << " GetSol ..........u.B.................Ru.... 1 7 RedWall" << endl;
#else
    cout << "- board string: 1x36 string showing the board" << endl;
    cout << "- #b: integer, the number of the opponent's blue pieces" << endl;
    cout << "- max #action: the depth limit of search (optional)" << endl;
    cout << "- RedWall: whether to check red-wall (optional)" << endl;
    cout << "Example: " << prog_name << " GetSol ..........u.B.................Ru.... 1 7 RedWall" << endl;
#endif
}

void GetSolution(const vector<string>& argvVec) {
    if ((argvVec.size() < 3) || (argvVec[1].length() != 36)) {
        HelpMessageGetSol(argvVec[0]);
        return;
    }

    string board = argvVec[1];
    bool is_partly_revealed = ((board.find("r") != string::npos)
        || (board.find("b") != string::npos));
    string mode = (is_partly_revealed ? string("p") : string("n"));
    int eB = stoi(argvVec[2]);
    int maxAction = 51;
    if ((argvVec.size() >= 4)
        && (argvVec[3].find_first_of("0123456789") != string::npos))
    {
        maxAction = stoi(argvVec[3]);
    }
    string subMode = (is_partly_revealed ? string("a") : string("n"));

    if (is_partly_revealed) {
#if OUTPUT_JP_MESSAGE
        cout << "一部公開" << endl;
#else
        cout << "partly-revealed" << endl;
#endif
    } else {
#if OUTPUT_JP_MESSAGE
        cout << "非公開" << endl;
#else
        cout << "not-revealed" << endl;
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
    cout << search.returnCount() << " ノード数" << endl;
    cout << search.returnLastCount() << " 必勝手数ノード数" << endl;
#else
    cout << search.returnCount() << " nodes" << endl;
    cout << search.returnLastCount() << " nodes of #winning move" << endl;
#endif

#if OUTPUT_JP_MESSAGE
    cout << actionNum << " 手詰" << endl;
#else
    cout << actionNum << " moves" << endl;
#endif

    if (mode == string("p")) {
        Search subSearchBlueCapture(mode, "a");
        int actionNumBlueCapture = subSearchBlueCapture.think(board, eB, actionNum, actionNum - 1);
        if (actionNumBlueCapture == 0) {
#if OUTPUT_JP_MESSAGE
            cout << "<青駒全取り>" << endl;
#else
            cout << "<Capture-Win>" << endl;
#endif
        } else {
            search.think(board, eB, maxAction, actionNum - 1);
        }
    }
    bool needCheckRedWall = false;
    for (size_t i = 3; i < argvVec.size(); i++) {
        if (argvVec[i] == string("RedWall")) { needCheckRedWall = true; }
    }
    if (needCheckRedWall) {
        Search subSearchRedWall(mode, "r");
        int actionNumRedWall = subSearchRedWall.think(board, eB, actionNum);
        if (actionNumRedWall == 0) {
#if OUTPUT_JP_MESSAGE
            cout << "<赤駒壁利用>" << endl;
#else
            cout << "<Red-Wall>" << endl;
#endif
        }
    }
#if OUTPUT_JP_MESSAGE
    cout << "解答例" << endl;
#else
    cout << "Example Solution" << endl;
#endif
    cout << search.answerBoard << endl;
}


