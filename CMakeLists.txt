# 設定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.15)

# 設定你的專案名稱
project(GeisterEndgamePuzzle)
# 1. 啟用 FetchContent 模組 (CMake 內建的下載神器)
include(FetchContent)

set(CPR_ENABLE_LIBPSL OFF CACHE BOOL "Disable libpsl in cpr" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "Disable libpsl in curl" FORCE)

# 2. 告訴 CMake 去 GitHub 下載 cpr (它會連 libcurl 一起自動搞定)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.12.0  # 使用穩定版本
)


FetchContent_MakeAvailable(cpr)

# 3. 設定你的 C++ 執行檔 (假設你的程式碼寫在 CprDemo.cpp 或 main.cpp)
# 注意：這裡的 CprDemo.cpp 請改成你實際的 .cpp 檔名
add_executable(GeisterEndgamePuzzle main.cpp CsvToJson.cpp)

# 4. 把 cpr 函式庫連結到你的程式上
target_link_libraries(GeisterEndgamePuzzle PRIVATE cpr::cpr)