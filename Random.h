#pragma once
#include "IncludeLib.h"

class IntRandom {
private:
	// 乱数生成器
	static std::mt19937_64 mt64;

public:
	static void reset(int random_seed) { mt64 = std::mt19937_64(random_seed); }

	// [min, max]
	static uint64_t get_rand_range(uint64_t min_val, uint64_t max_val) {
		// [min_val, max_val] の一様分布整数 (int) の分布生成器
		std::uniform_int_distribution<int> get_rand_uni_int(min_val, max_val);

		// 乱数を生成
		return get_rand_uni_int(mt64);
	}
};
