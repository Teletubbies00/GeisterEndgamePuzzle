#pragma once
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>  // for std::unordered_map
#include <algorithm>
#include <random>
#include <chrono>
#include <time.h>
#include <cstdlib>
#include <cstdio>
#if defined(_MSC_VER)
#include <Windows.h>
#include <direct.h>
#include <intrin.h>
#elif defined(__GNUC__)
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#define sprintf_s(buf, ...) snprintf((buf), sizeof(buf), __VA_ARGS__)
#endif

#define OUTPUT_JP_MESSAGE 0

using namespace std;

inline int makeDir(const char* dirName) {
#if defined(_MSC_VER)
	return _mkdir(dirName);
#elif defined(__GNUC__)
	return mkdir(dirName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

inline int changeDir(const char* dirName) {
#if defined(_MSC_VER)
	return _chdir(dirName);
#elif defined(__GNUC__)
	return chdir(dirName);
#endif
}
