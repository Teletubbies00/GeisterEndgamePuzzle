#define WIN32_LEAN_AND_MEAN

#include "RandomGeneration.h"
#include "ExampleSolution.h"
#include "CsvToJson.h"
#include <direct.h>
#include "httplib.h"
#pragma comment(lib, "ws2_32.lib")
std::mt19937_64 IntRandom::mt64;

#include"answerGenerator.h"
using namespace std;
// 處理資料
//#define COMMUNICATE_WITH_WEBSITE


int main(int argc, char* argv[]) {
    // -------------------------------//
    //            新增功能             //
    // -------------------------------//
    //       傳輸Json給網站功能
    #ifdef COMMUNICATE_WITH_WEBSITE
        using ordered_json = nlohmann::ordered_json;
        ordered_json allPuzzles = Data::convertCsvToJson("test.csv", "result.json");
        if (allPuzzles == NULL) {
            cout << "ERROR : No Data in test.csv" << endl;
            return 0;
        }
        std::string json_payload = allPuzzles.dump();

        // 連接本地端port 8000
        httplib::Client cli("http://127.0.0.1:8000");

        std::cout << "Sending JSON file to FastAPI..." << std::endl;

        // 3. 發送 POST 請求給 "/upload" 路由，並指定 Content-Type 為 application/json
        if (auto res = cli.Post("/upload", json_payload, "application/json")) {
            // 檢查 HTTP 狀態碼 (200 代表 OK)
            if (res->status == 200) {
                std::cout << "success" << std::endl;
                std::cout << "FastAPI : " << res->body << std::endl;
            }
            else {
                std::cout << "failed: " << res->status << std::endl;
            }
        }
        else {
            // 處理底層連線錯誤 (例如 FastAPI 根本沒開)
            auto err = res.error();
            std::cout << "error : " << httplib::to_string(err) << std::endl;
            std::cout << "Check if fastapi is running" << std::endl;
        }
        return 0;
    #endif // 0
    if (argv[1] == string("SolveCsvPuzzle")) {
        string inputCsv = "questions.csv";  // 你的題目檔名
        string outputCsv = "answers.csv";   // 你想要的輸出檔名
        string mode = "n";                  // normal 模式
        SolveFromCSV(inputCsv, outputCsv, mode);
    }
    return 0;
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
