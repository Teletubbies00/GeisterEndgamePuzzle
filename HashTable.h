#pragma once
#include "IncludeLib.h"

// 效能分析 (Profiling) 用的全域變數，用來統計查表/寫表的次數與花費時間 // 增加inline
inline unsigned int hashlooktime = 0;
inline unsigned int hashlookcount = 0;
inline unsigned int hashputtime = 0;

inline long long mask = 1;  // 用於生成雜湊鍵值的遮罩 (Mask)，這裡設為 1，稍後會用 XOR (^) 運算稍微打亂位元

struct Node;  // 前向宣告


// ======================================
// Hash Combine (雜湊合成函數)
// ======================================

// 因為我們的盤面由多個 Bitboard 組成，我們需要把多個雜湊值「攪拌」在一起變成一個。
// 這裡使用的是知名 C++ 函式庫 Boost (boost::hash_combine) 的經典魔術數字演算法
template<typename T>
void hash_combine(size_t& seed, T const& v) {
	// 基本型別 (如 long long, int) 的雜湊生成，使用標準函式庫提供的方法
	std::hash<T> primitive_type_hash;

	// 將新生成的雜湊值與舊的 seed 混合。
	// 0x9e3779b9 是黃金比例的魔術數字，能有效打亂位元，極大程度避免雜湊碰撞 (Hash Collision)
	seed ^= primitive_type_hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


//======================================
// Keys
//======================================

/*
 * 作為雜湊表 (Hash Table) Key 的物件
 * 它儲存了足以「唯一代表一個盤面狀態」的所有資訊。
 */
struct HashTableKey {
	long long v;   // 對應 existB (己方青鬼)
	long long w;   // 對應 existR (己方赤鬼
	long long x;   // 對應 existP (敵方紫鬼/未知)
	long long y;   // 對應 existEB (敵方青鬼)
	long long z;   // 對應 existER (敵方赤鬼)
	int depth;     // 搜尋深度 (同一個盤面，如果剩餘搜尋深度不同，價值也不同，所以要當作 Key 的一部分)

	struct Hash;  // 內部結構：定義如何對這個 Key 產生 Hash 值

	HashTableKey(long long v, long long w, long long x, long long y, long long z, int depth);
	bool operator==(const HashTableKey& rhs) const;
	bool operator!=(const HashTableKey& rhs) const;

private:
	friend std::ostream& operator<<(std::ostream& s, const HashTableKey& obj);
};

/*
 * 建構子實作：將傳入的盤面狀態存起來
 */
inline HashTableKey::HashTableKey(long long v, long long w, long long x, long long y, long long z, int depth) {
	this->v = v;
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
	this->depth = depth;
	return;
}

/**
 * 比較運算子 == 實作：
 * 只有當 5 個 Bitboard 和 depth 都一模一樣時，才視為同一個盤面狀態
 */
inline bool HashTableKey::operator==(const HashTableKey& rhs) const {
	const HashTableKey& lhs = *this;
	return lhs.v == rhs.v && lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.depth == rhs.depth;
}

/**
 * 比較運算子 != 實作
 */
inline bool HashTableKey::operator!=(const HashTableKey& rhs) const {
	return !(this->operator==(rhs));
}

/**
 * 自訂雜湊函數 (Hash Function) 的實作
 * unordered_std::map 需要知道怎麼把 HashTableKey 轉成一個 size_t 數字
 */
struct HashTableKey::Hash {
	typedef size_t result_type;

	size_t operator()(const HashTableKey& key) const;
};

inline std::size_t HashTableKey::Hash::operator()(const HashTableKey& key) const {
	size_t seed = 0;
	// 依序將 5 個 Bitboard 和 depth 丟進去攪拌 (hash_combine)
	hash_combine(seed, key.v);
	hash_combine(seed, key.w);
	hash_combine(seed, key.x);
	hash_combine(seed, key.y);
	hash_combine(seed, key.z);
	hash_combine(seed, key.depth);

	return seed;  // 回傳最終攪拌出來的唯一識別碼
}

//======================================
// Hash Table
//======================================

/**
 * 定義 Hashstd::map 型別：
 * 使用 C++ 標準庫的 unordered_std::map。
 * 鍵 (Key) 是 HashTableKey，值 (Value) 是 Node* 指標，並套用我們剛剛自訂的雜湊函數。
 */
typedef std::unordered_map<HashTableKey, Node*, HashTableKey::Hash> Hashmap;

//======================================
// Sample Code (查表與寫表操作)
//======================================


// 在雜湊表中尋找是否算過這個盤面 // 增加inline
inline Node* LookUpHash(Hashmap& map, BitBoard& bb, int depth) {
	hashlookcount += 1;
	clock_t start = clock();

	// 建立 Key。這裡用 ^ mask (XOR 1) 稍微翻轉了最低位元
	HashTableKey key(bb.existB ^ mask, bb.existR ^ mask, bb.existP ^ mask, bb.existEB ^ mask, bb.existER ^ mask, depth);

	// 在 std::map 中尋找這個 key
	auto itr = map.find(key);

	if (itr != map.end()) {
		// 找到了！(Cache Hit)
		clock_t end = clock();
		hashlooktime += (end - start);
		/*bb.printBoard();
		std::cout << depth << std::endl;
		system("PAUSE");*/
		return (itr->second);  // 回傳存好的 Node 指標
	}
	else {
		// 沒找到 (Cache Miss)
		clock_t end = clock();
		hashlooktime += (end - start);
		return NULL;
	}
}

// 將算好的盤面存入雜湊表 // 增加inline
inline void PutInHash(Hashmap& map, BitBoard& bb, int depth, Node* np) {
	clock_t start = clock();

	// 建立 Key
	HashTableKey key(bb.existB ^ mask, bb.existR ^ mask, bb.existP ^ mask, bb.existEB ^ mask, bb.existER ^ mask, depth);

	// 建立 Key-Value 的組合
	std::pair<HashTableKey, Node*> keyValue = std::pair<HashTableKey, Node*>(key, std::move(np));

	map.insert(move(keyValue));

	clock_t end = clock();
	hashputtime += (end - start);
}

// 取得雜湊表大小 // 增加inline
inline size_t CountHashSize(Hashmap map) {
	return map.size();
}

// 全域節點計數器 // 增加inline
inline unsigned int nodeCount = 1;
