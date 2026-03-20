//i行j列目(i>=0, j>=0)をマスi * 6 + jとおく.
//用意するメソッド：終了条件(2種類), 動かす
#pragma once
#include "IncludeLib.h"

// 這是「著法排序（Move Ordering）」的陣列。
// 在 AI 搜尋樹中，如果先嘗試「好」的步數，可以更快觸發 Alpha-Beta 剪枝。
// 這裡預先定義了兩個陣列（對應雙方），依照特定優先級排出 36 個格子的順序。
int moveOrderArray[2][36] = { { 0,5,30,35,1,6,4,11,24,31,29,34,2,7,12,3,10,17,18,25,32,23,28,33,8,13,9,16,19,26,22,27,14,15,20,21 },
							  { 30,35,0,5,24,31,29,34,1,6,4,11,18,25,32,23,28,33,2,7,12,3,10,17,22,27,14,15,20,21,8,13,9,16,19,26 } };

// 下面這組常數是「位元遮罩（Bit Mask）」。
// 棋盤格編號為 0~35。例如 0x21 換算成二進位是 0...0010 0001，代表第 0 格和第 5 格。
// 0000 0000 0000 0000 0000 0000 0000 0010 0001 == 0x21
const long long playerExitBB = 0x21ll;
// 1000 0100 0000 0000 0000 0000 0000 0000 0000 == 0x840000000
const long long oppExitBB = 0x840000000ll;
// 1111 1111 1111 1111 1100 0000 0000 0000 0000 == 0xffffc0000
const long long playerZoneBB = 0xffffc0000ll;
// 0000 0000 0000 0000 0011 1111 1111 1111 1111 == 0x3ffff
const long long oppZoneBB = 0x3ffffll;
// 1110 0011 1000 1110 0011 1000 1110 0011 1000 == 0xe38e38e38;
const long long leftZoneBB = 0xe38e38e38ll;

// Manhattan Distance（曼哈頓距離）表。預先算好任兩格之間的最短步數，節省計算時間。
int manhattanDistance[36][36];
void initializeManhattanDistance() {
	for (int grid1 = 0; grid1 < 36; grid1++) {
		int x1 = grid1 % 6;
		int y1 = grid1 / 6;
		for (int grid2 = 0; grid2 < 36; grid2++) {
			int x2 = grid2 % 6;
			int y2 = grid2 / 6;
			int dist = abs(x1 - x2) + abs(y1 - y2);
			manhattanDistance[grid1][grid2] = dist;
		}
	}
}


// 計算有幾個 bit 是 1（即：盤面上還有幾顆這種類型的棋子）
//ビットカウント
inline int bitCount(long long x) {
#if defined(_MSC_VER)
	return int(__popcnt64(x));
#elif defined(__GNUC__)
	__asm__("popcnt %1, %0" : "=r" (x) : "r" (x));
	return x;
#endif
}

// 找到最低位的 1 在哪裡（即：找棋盤上編號最小的棋子位置）
inline int lsb(long long x) {
#if defined(_MSC_VER)
	unsigned long index;
	_BitScanForward64(&index, x);
	return (int)index;
#elif defined(__GNUC__)
	unsigned long long index;
	__asm__("bsfq %1, %0": "=r"(index) : "rm"(x));
	return (int)index;
#endif
}

// 找到最低位的 1 的位置，並將那個 1 變成 0（即：取出並移除該棋子）
inline int popLsb(long long& b) {
	const int index = lsb(b);
	//b &= ~(1<<index);
	b &= (b - 1);
	return index;
}

struct BitBoard {
public:

	// 使用 5 個 64-bit 整數來記錄五種棋子的位置。
	// 若第 i 個 bit 為 1，代表第 i 格有該種棋子。
	long long existR;	//マスiに自分の赤がある ⇔ (existR >> i) & 1 == 1とする。
	long long existB;
	long long existP;	//マスiに敵の駒がある ⇔ (existP >> i) & 1 == 1とする。Purple駒…取ったら赤に変化し, 脱出するときは青に変化する.
	long long existEB;
	long long existER;

	// 將字串格式 ("......B....u...") 轉換成 Bitboard 格式
	//board[i]…マスiにある駒の種類(R, B, u)
	void toBitBoard(string board) {
		existR = existB = existP = existEB = existER = 0;
		for (int i = 0; i < 36; i++) {
			if (board[i] == 'R') { existR |= (1LL << i); }
			if (board[i] == 'B') { existB |= (1LL << i); }
			if (board[i] == 'u') { existP |= (1LL << i); }
			if (board[i] == 'b') { existEB |= (1LL << i); }
			if (board[i] == 'r') { existER |= (1LL << i); }
		}
	}

	// 將 Bitboard 轉回字串格式，方便除錯或輸出
	string returnString() {
		string board = ".................................... ";
		for (int i = 0; i < 36; i++) {
			if ((existB >> i) & 1LL) board[i] = 'B';
			if ((existR >> i) & 1LL) board[i] = 'R';
			if ((existP >> i) & 1LL) board[i] = 'u';
			if ((existEB >> i) & 1LL) board[i] = 'b';
			if ((existER >> i) & 1LL) board[i] = 'r';
		}
		return board;
	}

	// 產生所有合法的移動步數
	//手の生成. 手の個数を返す(teban=0 : 自分手番, teban=1:敵手番). from[], to[]に手を格納. (from, toはサイズ32以上の配列）
	//kiki[5 * i + j] = マスiと隣接するj番目のマスの番号. なければ-1。
	int makeMoves(int teban, int kiki[], int from[], int to[]) {
		int pos, i, cnt = 0;
		for (pos = 0; pos < 36; pos++) {
			if (teban == 0 && !(((existR | existB) >> pos) & 1)) continue;
			if (teban == 1 && !(((existP | existEB | existER) >> pos) & 1)) continue;
			for (i = pos * 5; kiki[i] != -1; i++) {
				int npos = kiki[i];
				//マスpos -> マスnposと駒を動かせるか？（自駒とぶつからないか）
				if (teban == 0 && (((existR | existB) >> npos) & 1)) continue;
				if (teban == 1 && (((existP | existEB | existER) >> npos) & 1)) continue;
				//動かせる
				from[cnt] = pos;
				to[cnt] = npos;
				cnt++;
			}
		}
		return cnt;
	}

	// 根據前面定義好的 moveOrderArray，對產生的步數進行重新排序（優化搜尋效率）
	void moveOrder(int teban, int moveNum, int from[], int to[]) {
		int orderFrom[36];
		int orderTo[36];
		int cnt = 0;
		for (int i = 0; i < 36; i++) {
			for (int j = 0; j < moveNum; j++) {
				if (moveOrderArray[teban][i] == to[j]) {
					orderFrom[cnt] = from[j];
					orderTo[cnt] = to[j];
					cnt += 1;
					if (cnt == moveNum) break;
				}
			}
			if (cnt == moveNum) break;
		}
		for (int k = 0; k < moveNum; k++) {
			from[k] = orderFrom[k];
			to[k] = orderTo[k];
		}

	}

	//先手の赤を動かした後の状態に更新する。
	inline void moveR(int from, int to) {
		existR &= ~(1LL << from);
		existR |= (1LL << to);
		existEB &= ~(1LL << to);
		existER &= ~(1LL << to);
		existP &= ~(1LL << to);
	}

	inline void moveB(int from, int to) {
		existB &= ~(1LL << from);
		existB |= (1LL << to);
		existEB &= ~(1LL << to);
		existER &= ~(1LL << to);
		existP &= ~(1LL << to);
	}

	inline void moveP(int from, int to) {
		existP &= ~(1LL << from);
		existP |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	inline void moveEB(int from, int to) {
		existEB &= ~(1LL << from);
		existEB |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	inline void moveER(int from, int to) {
		existER &= ~(1LL << from);
		existER |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	void move(int from, int to) {
		if ((existR >> from) & 1)
			moveR(from, to);
		else if ((existB >> from) & 1)
			moveB(from, to);
		else if ((existEB >> from) & 1)
			moveEB(from, to);
		else if ((existER >> from) & 1)
			moveER(from, to);
		else if ((existP >> from) & 1)
			moveP(from, to);
		else {
			cout << "error board" << endl;
			printBoard();
			cout << "from = " << from << "to = " << to << endl;
			assert(0);
		}

	}

	int check(int player, int pnum, const string& subMode) {
		//どちらが勝ち状態かを返す. (0,2…自分, 1,3…敵, 4…不明). 紫駒がpnum個以下なら敵の勝ち.
		if (existB == 0 || (existER == 0 && bitCount(existP | existEB) <= pnum)) return 1;
		bool myRedAllCaptured = (existR == 0);
		bool enBlueAllCaptured = (existEB == 0 && existER != 0 && existP == 0);
		if (subMode == "r") {
			if (enBlueAllCaptured) { return 0; }
		}
		else {
			if (myRedAllCaptured || enBlueAllCaptured) { return 0; }
		}
		if (player == 0 && ((existB & playerExitBB) != 0)) return 2;
		if (player == 1 && (((existP | existEB) & oppExitBB) != 0)) return 3;
		return 4;
	}

	// 先手側の脱出口と一番近い先手側青駒との距離
	// 探索打ち切り用・特徴量用
	int minEscapeAction(void) {
		int minAction = 100;
		long long b = existB;
		while (b != 0) {
			int id = popLsb(b);
			int dist = min(manhattanDistance[id][0], manhattanDistance[id][5]);
			if (minAction > dist) minAction = dist;
		}
		return minAction;
	}

	// 敵青・紫駒を全て取るのにかかる（両方含む）手数の下限値
	// 探索打ち切り用
	int minCaptureAction(void) {
		long long oppB = existP | existEB;
		int farestToOppDistance = 0;
		int overallNearestDistance = 100;
		while (oppB != 0) {
			int id1 = popLsb(oppB);
			long long b = (existB | existR);
			int nearestToOppDistance = 100;
			while (b != 0) {
				int id2 = popLsb(b);
				int dist = manhattanDistance[id1][id2];
				nearestToOppDistance = min(nearestToOppDistance, dist);
				overallNearestDistance = min(overallNearestDistance, dist);
			}
			farestToOppDistance = max(farestToOppDistance, nearestToOppDistance);
		}
		overallNearestDistance += (bitCount(existP | existEB) - 1) * 2;
		return max(overallNearestDistance, farestToOppDistance);
	}

	//デバッグ用
	void printBoard() {
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				int id = y * 6 + x;
				if ((existR >> id) & 1) cout << "R";
				else if ((existB >> id) & 1) cout << "B";
				else if ((existP >> id) & 1) cout << "P";
				else if ((existER >> id) & 1) cout << "r";
				else if ((existEB >> id) & 1) cout << "b";
				else cout << ".";
			}
			cout << endl;
		}
		cout << endl;
	}
};
