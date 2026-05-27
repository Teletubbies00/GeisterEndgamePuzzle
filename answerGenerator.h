#pragma once
#include"Search.h"
#include"CsvParser.h"

class answerGenerator {
public:
	std::vector<Data::Puzzle> puzzleSet;
public:
	void solve(const std::string& outputCsvPath, const std::string& mode);
	void loadPuzzleSet(const std::string& puzzleSetPath);
};