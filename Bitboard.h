// 假設 i 行 j 列 (i>=0, j>=0) 為棋盤的第 i * 6 + j 格（共 0~35 格）。
// 準備的方法包含：結束條件判定 (2種類型)、移動棋子
#pragma once
#include "IncludeLib.h"

// 這是「著法排序（Move Ordering）」的陣列。
// 在 AI 搜尋樹中，如果先嘗試「好」的步數，可以更快觸發 Alpha-Beta 剪枝。
// 這裡預先定義了兩個陣列（對應雙方），依照特定優先級排出 36 個格子的順序。 // 加上inline
inline int moveOrderArray[2][36] = { { 0,5,30,35,1,6,4,11,24,31,29,34,2,7,12,3,10,17,18,25,32,23,28,33,8,13,9,16,19,26,22,27,14,15,20,21 },
							  { 30,35,0,5,24,31,29,34,1,6,4,11,18,25,32,23,28,33,2,7,12,3,10,17,22,27,14,15,20,21,8,13,9,16,19,26 } };

// 下面這組常數是「位元遮罩（Bit Mask）」。
// 棋盤格編號為 0~35。例如 0x21 換算成二進位是 0...0010 0001，代表第 0 格和第 5 格。
// 0000 0000 0000 0000 0000 0000 0000 0010 0001 == 0x21  // 加上inline
inline const long long playerExitBB = 0x21ll;
// 1000 0100 0000 0000 0000 0000 0000 0000 0000 == 0x840000000 // 加上inline
inline const long long oppExitBB = 0x840000000ll;
// 1111 1111 1111 1111 1100 0000 0000 0000 0000 == 0xffffc0000 // 加上inline
inline const long long playerZoneBB = 0xffffc0000ll;
// 0000 0000 0000 0000 0011 1111 1111 1111 1111 == 0x3ffff // 加上inline
inline const long long oppZoneBB = 0x3ffffll;
// 1110 0011 1000 1110 0011 1000 1110 0011 1000 == 0xe38e38e38; // 加上inline
inline const long long leftZoneBB = 0xe38e38e38ll;

// Manhattan Distance（曼哈頓距離）表。預先算好任兩格之間的最短步數，節省計算時間。 // 加上inline
inline int manhattanDistance[36][36];
inline void initializeManhattanDistance() { // 加上inline
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
// 利用編譯器內建的硬體指令 popcnt 達到極速計算
inline int bitCount(long long x) {
#if defined(_MSC_VER)
	return int(__popcnt64(x));
#elif defined(__GNUC__)
	__asm__("popcnt %1, %0" : "=r" (x) : "r" (x));
	return x;
#endif
}

// 找到最低位的 1 在哪裡（即：找棋盤上編號最小的棋子位置）
// 同樣利用硬體指令 BitScanForward / bsfq 加速
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

// 找到最低位的 1 的位置，並將那個 1 變成 0（即：取出並把該棋子從遮罩中移除）
// 這在歷遍所有棋子時非常常用（while迴圈搭配 popLsb）
inline int popLsb(long long& b) {
	const int index = lsb(b);
	//b &= ~(1<<index);
	b &= (b - 1);
	return index;
}

struct BitBoard {
public:

	// 使用 5 個 64-bit 整數來記錄五種棋子的位置。
	// 如果 (existR >> i) & 1 == 1，代表第 i 格有己方的紅棋。
	long long existR;	// 己方紅棋 (Red) -> 通常是壞幽靈
	long long existB;   // 己方青/藍棋 (Blue) -> 通常是好幽靈
	long long existP;	// 敵方的紫棋 (Purple) -> 未翻開/未知的幽靈。被吃掉時會根據真實身份變成敵方紅或青棋。
	long long existEB;  // 敵方青/藍棋 (Enemy Blue)
	long long existER;  // 敵方紅棋 (Enemy Red)

	// 將字串格式 ("......B....u...") 轉換成 Bitboard 格式
	// board[i] 表示第 i 格的棋子種類
	void toBitBoard(std::string board) {
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
	std::string returnstring() {
		std::string board = ".................................... ";
		for (int i = 0; i < 36; i++) {
			if ((existB >> i) & 1LL) board[i] = 'B';
			if ((existR >> i) & 1LL) board[i] = 'R';
			if ((existP >> i) & 1LL) board[i] = 'u';
			if ((existEB >> i) & 1LL) board[i] = 'b';
			if ((existER >> i) & 1LL) board[i] = 'r';
		}
		return board;
	}

	// 產生所有合法的移動步數，回傳可走步數的總數。
	// teban = 回合 (0: 己方回合, 1: 敵方回合)
	// from[], to[] 會把產生的起點與終點存進去。
	int makeMoves(int teban, int kiki[], int from[], int to[]) {
		int pos, i, cnt = 0;
		for (pos = 0; pos < 36; pos++) {
			// 如果輪到己方，但該格沒有己方的棋 (R或B)，就跳過
			if (teban == 0 && !(((existR | existB) >> pos) & 1)) continue;
			// 如果輪到敵方，但該格沒有敵方的棋 (P或EB或ER)，就跳過
			if (teban == 1 && !(((existP | existEB | existER) >> pos) & 1)) continue;
			
			// kiki 陣列存了每一格相鄰的格子（上下左右），-1 代表碰壁
			for (i = pos * 5; kiki[i] != -1; i++) {
				int npos = kiki[i];
				// 檢查是否會吃到自己的棋子（不能踩在自己的棋子上）
				if (teban == 0 && (((existR | existB) >> npos) & 1)) continue;
				if (teban == 1 && (((existP | existEB | existER) >> npos) & 1)) continue;
				
				// 是一步合法的移動，記錄下來
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

	// --- 下列是具體的移動執行函式 ---
	// 邏輯：清除起點 (~(1LL << from))，佔領終點 (|= (1LL << to))
	// 同時清除終點上可能存在的敵方棋子（吃子邏輯）


	// 移動己方紅棋
	inline void moveR(int from, int to) {
		existR &= ~(1LL << from);
		existR |= (1LL << to);
		existEB &= ~(1LL << to);
		existER &= ~(1LL << to);
		existP &= ~(1LL << to);
	}

	// 移動己方青棋
	inline void moveB(int from, int to) {
		existB &= ~(1LL << from);
		existB |= (1LL << to);
		existEB &= ~(1LL << to);
		existER &= ~(1LL << to);
		existP &= ~(1LL << to);
	}

	// 移動敵方紫棋
	inline void moveP(int from, int to) {
		existP &= ~(1LL << from);
		existP |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	// 移動敵方青棋
	inline void moveEB(int from, int to) {
		existEB &= ~(1LL << from);
		existEB |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	// 移動敵方紅棋
	inline void moveER(int from, int to) {
		existER &= ~(1LL << from);
		existER |= (1LL << to);
		existR &= ~(1LL << to);
		existB &= ~(1LL << to);
	}

	// 通用的移動介面，會自動判斷起點是哪種棋子並呼叫對應的 move 函式
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
			std::cout << "error board" << std::endl;
			printBoard();
			std::cout << "from = " << from << "to = " << to << std::endl;
			assert(0);
		}

	}

	// 判斷遊戲是否結束，回傳勝利方
	// 回傳值：0,2 代表己方贏；1,3 代表敵方贏；4 代表尚未結束
	// pnum：用來判斷未知紫棋(P)的數量
	int check(int player, int pnum, const std::string& subMode) {
		// 敵方獲勝條件：己方青棋全被吃光，或敵方紅棋死光且剩下的青+紫棋數小於等於 pnum
		if (existB == 0 || (existER == 0 && bitCount(existP | existEB) <= pnum)) return 1;
		bool myRedAllCaptured = (existR == 0);
		bool enBlueAllCaptured = (existEB == 0 && existER != 0 && existP == 0);
		
		// 特殊規則模式 ("r" 模式)
		if (subMode == "r") {
			if (enBlueAllCaptured) { return 0; }
		}
		else {
			// 一般己方獲勝條件：己方紅棋被吃光 (敵方吃到壞幽靈)，或敵方青棋被吃光
			if (myRedAllCaptured || enBlueAllCaptured) { return 0; }
		}

		// 脫出條件：己方青棋踩在己方脫出口
		if (player == 0 && ((existB & playerExitBB) != 0)) return 2;
		// 脫出條件：敵方青/紫棋踩在敵方脫出口
		if (player == 1 && (((existP | existEB) & oppExitBB) != 0)) return 3;

		return 4; // 繼續遊戲
	}

	// 計算己方青色（藍色）幽靈距離「脫出口」的最短距離
	// 用途 1：探索剪枝 (Pruning)。如果最短距離大於剩餘的搜尋深度，代表絕對來不及脫出，直接砍掉該分支。
	// 用途 2：特徵量 (Feature)。可以作為評估盤面好壞的依據。
	int minEscapeAction(void) {
		int minAction = 100;   // 初始設一個極大值
		long long b = existB;  // 取得己方所有青鬼的位元棋盤 (Bitboard)

		// 只要盤面上還有青鬼
		while (b != 0) {
			// popLsb: 拔出並回傳最右邊的 1 (Least Significant Bit) 的位置 ID
			// 也就是快速找出下一個青鬼所在的格子編號
			int id = popLsb(b);

			// 計算這個青鬼到左上角 (0) 和右上角 (5) 兩個脫出口的曼哈頓距離，取比較近的那個
			int dist = min(manhattanDistance[id][0], manhattanDistance[id][5]);

			// 更新目前盤面上所有青鬼中，距離脫出口最近的距離
			if (minAction > dist) minAction = dist;
		}
		return minAction;
	}

	// 計算「吃掉敵方所有青鬼與紫鬼(未翻牌/未知)」所需步數的理論下限值
	// 用途：探索剪枝。如果剩餘步數不夠吃光關鍵敵方棋子，直接停止這條路線的搜尋。
	int minCaptureAction(void) {
		// 鎖定目標：敵方的紫鬼 (existP) 與敵方的青鬼 (existEB)
		long long oppB = existP | existEB;
		// farestToOppDistance: 所有目標中，「最難被吃掉」(距離最近的己方棋子最遠) 的距離
		int farestToOppDistance = 0;
		// overallNearestDistance: 盤面上「隨便吃掉一隻目標」的最短距離
		int overallNearestDistance = 100;

		// 遍歷每一個敵方目標
		while (oppB != 0) {
			int id1 = popLsb(oppB);  // 抓出一隻敵方目標的位置
			long long b = (existB | existR);  // 取得己方所有的棋子 (青鬼 + 赤鬼)

			// 計算「這隻敵方目標」距離「最近的己方棋子」有多遠
			int nearestToOppDistance = 100;

			while (b != 0) {
				int id2 = popLsb(b);  // 抓出己方棋子
				int dist = manhattanDistance[id1][id2];  // 查表取得曼哈頓距離
				nearestToOppDistance = min(nearestToOppDistance, dist);
				overallNearestDistance = min(overallNearestDistance, dist);
			}
			// 更新「最難被吃掉」的目標距離
			farestToOppDistance = max(farestToOppDistance, nearestToOppDistance);
		}
		// 理論下限步數計算：
		// 要吃掉所有目標，至少要先花 overallNearestDistance 吃掉第一隻。
		// 剩下的目標，假設每隻至少要多花 2 步 (己方走過去 1 步 + 敵方可能逃跑或位置轉換)。
		overallNearestDistance += (bitCount(existP | existEB) - 1) * 2;

		// 回傳兩者中的最大值，這個值一定非常樂觀 (Underestimation)，非常適合用來做 A* 或 Alpha-Beta 的安全剪枝
		return max(overallNearestDistance, farestToOppDistance);
	}

	// 終端機除錯用的印出盤面函式 (將 64-bit 整數轉換為 6x6 視覺化字元)
	void printBoard() {
		// 從左上角 (y=0, x=0) 印到右下角 (y=5, x=5)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				int id = y * 6 + x;  // 格子 ID (0 ~ 35)

				if ((existR >> id) & 1) std::cout << "R";
				else if ((existB >> id) & 1) std::cout << "B";
				else if ((existP >> id) & 1) std::cout << "P";
				else if ((existER >> id) & 1) std::cout << "r";
				else if ((existEB >> id) & 1) std::cout << "b";
				else std::cout << ".";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};
