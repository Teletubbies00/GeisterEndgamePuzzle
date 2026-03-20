#pragma once
#include "IncludeLib.h"

unsigned int hashlooktime = 0;
unsigned int hashlookcount = 0;
unsigned int hashputtime = 0;

long long mask = 1;

struct Node;

template<typename T>
void hash_combine(size_t& seed, T const& v) {
	//基本型に関するハッシュ生成は標準ライブラリが提供している
	std::hash<T> primitive_type_hash;

	//生成したハッシュを合成する。このコードはboostものを使用する
	seed ^= primitive_type_hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//======================================
// Keys
//======================================

/**
 * ハッシュテーブルのキーとして使用するオブジェクト
 */
struct HashTableKey {
	long long v;
	long long w;
	long long x;
	long long y;
	long long z;
	int depth;

	struct Hash;

	HashTableKey(long long v, long long w, long long x, long long y, long long z, int depth);
	bool operator==(const HashTableKey& rhs) const;
	bool operator!=(const HashTableKey& rhs) const;

private:
	friend ostream& operator<<(ostream& s, const HashTableKey& obj);
};

/**
 * コンストラクタ
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
 * 比較演算子
 */
inline bool HashTableKey::operator==(const HashTableKey& rhs) const {
	const HashTableKey& lhs = *this;
	return lhs.v == rhs.v && lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.depth == rhs.depth;
}

/**
 * 比較演算子
 */
inline bool HashTableKey::operator!=(const HashTableKey& rhs) const {
	return !(this->operator==(rhs));
}

/**
 * ハッシュ関数
 */
struct HashTableKey::Hash {
	typedef size_t result_type;

	size_t operator()(const HashTableKey& key) const;
};

inline std::size_t HashTableKey::Hash::operator()(const HashTableKey& key) const {
	size_t seed = 0;
	hash_combine(seed, key.v);
	hash_combine(seed, key.w);
	hash_combine(seed, key.x);
	hash_combine(seed, key.y);
	hash_combine(seed, key.z);
	hash_combine(seed, key.depth);

	return seed;
}

//======================================
// Hash Table
//======================================

/**
 * ハッシュテーブルの型
 */
typedef unordered_map<HashTableKey, Node*, HashTableKey::Hash> HashMap;

//======================================
// Sample Code
//======================================

Node* LookUpHash(HashMap& map, BitBoard& bb, int depth) {
	hashlookcount += 1;
	clock_t start = clock();

	HashTableKey key(bb.existB ^ mask, bb.existR ^ mask, bb.existP ^ mask, bb.existEB ^ mask, bb.existER ^ mask, depth);
	auto itr = map.find(key);

	if (itr != map.end()) {
		clock_t end = clock();
		hashlooktime += (end - start);
		/*bb.printBoard();
		cout << depth << endl;
		system("PAUSE");*/
		return (itr->second);
	}
	else {
		clock_t end = clock();
		hashlooktime += (end - start);
		return NULL;
	}
}

void PutInHash(HashMap& map, BitBoard& bb, int depth, Node* np) {
	clock_t start = clock();

	HashTableKey key(bb.existB ^ mask, bb.existR ^ mask, bb.existP ^ mask, bb.existEB ^ mask, bb.existER ^ mask, depth);
	pair<HashTableKey, Node*> keyValue = pair<HashTableKey, Node*>(key, move(np));

	map.insert(move(keyValue));

	clock_t end = clock();
	hashputtime += (end - start);
}

size_t CountHashSize(HashMap map) {
	return map.size();
}

unsigned int nodeCount = 1;
