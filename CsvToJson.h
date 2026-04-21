#pragma once
#include<string>
#include"json.hpp"
#include<filesystem>

namespace Data {
	using ordered_json = nlohmann::ordered_json; // 沒有key排序的，這樣question answer 順序不會顛倒
	using json = nlohmann::json;
	ordered_json convertCsvToJson(const std::string& csvPath, const std::string& jsonPath);
}
