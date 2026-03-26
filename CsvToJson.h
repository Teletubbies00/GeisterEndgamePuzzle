#pragma once
#include<string>
#include"json.hpp"

namespace Data {
	using json = nlohmann::json;
	using ordered_json = nlohmann::ordered_json; // 沒有key排序的，這樣question answer 順序不會顛倒
	void convertCsvToJson(const std::string& csvPath, const std::string& jsonPath);
}
