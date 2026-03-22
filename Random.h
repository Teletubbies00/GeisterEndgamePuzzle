#pragma once
#include "IncludeLib.h"

class IntRandom {
private:

	// 亂數生成器引擎 (Engine)
	static std::mt19937_64 mt64;

public:
	// 重置/初始化亂數種子 (Seed)
	// 通常會在程式剛啟動時呼叫一次，例如傳入當前時間，確保每次執行的亂數結果不同。
	static void reset(int random_seed) { mt64 = std::mt19937_64(random_seed); }

	// 取得指定範圍 [min_val, max_val] 內的隨機整數 (包含頭尾)
	// 問題:型別不一致
	static uint64_t get_rand_range(uint64_t min_val, uint64_t max_val) {
		// 定義一個「均勻分佈 (Uniform Distribution)」的規則，
		std::uniform_int_distribution<int> get_rand_uni_int(min_val, max_val);

		// 乱数を生成
		return get_rand_uni_int(mt64);
	}
};
