#include "RandomGeneration.h"
#include "ExampleSolution.h"
#include <direct.h>
std::mt19937_64 IntRandom::mt64;

int main(int argc, char* argv[]) {
    // -------------------------------//
    //            新增功能             //
    // -------------------------------//
    //       傳輸Json給網站功能
    #if COMMUNICATE_WITH_WEBSITE
        
        
        return 0;
    #endif // 0

    char buff[256];
    _getcwd(buff, 256);
    std::cout << "目前程式輸出的路徑在: " << buff << std::endl;



    initializeManhattanDistance();

    vector<string> argvVec;
    for (int i = 0; i < argc; i++) {
        if (i == 1) { continue; }
        argvVec.push_back(string(argv[i]));
    }
    const string modeKeywords[2] = { "RandomGen", "GetSol" };
    bool fit_mode_keyword = false;
    if (argc >= 2) {
        for (int i = 0; i < 2; i++) {
            if (string(argv[1]) == modeKeywords[i]) {
                fit_mode_keyword = true;
                break;
            }
        }
    }
    if (fit_mode_keyword == false) {
#if OUTPUT_JP_MESSAGE
        cout << "引数が正しくない" << endl;
        cout << "2 つのモード RandomGen GetSol それぞれの詳細を下記に参照してください．" << endl << endl;
#else
        cout << "Wrong program arguments" << endl;
        cout << "Please refer to the following for the details of the two modes, RandomGen and GetSol." << endl << endl;
#endif

#if OUTPUT_JP_MESSAGE
        cout << "ランダム生成法を回したい場合：" << endl;
#else
        cout << "To run random generation:" << endl;
#endif
        HelpMessageRandGen(argv[0]);
        cout << endl;

#if OUTPUT_JP_MESSAGE
        cout << "解答例を得たい場合：" << endl;
#else
        cout << "To obtain an example solution:" << endl;
#endif
        HelpMessageGetSol(argv[0]);
        cout << endl;
        return 0;
    }

    clock_t start = clock();
    if (argv[1] == string("RandomGen")) {
        RandomGeneration(argvVec);
    }
    if (argv[1] == string("GetSol")) {
        GetSolution(argvVec);
    }
    clock_t end = clock();
    cout << "Total time = " << double(end - start) / double(CLOCKS_PER_SEC) << " s" << endl;
}
