#pragma once
#include "IncludeLib.h"
#include "Bitboard.h"
#include "HashTable.h"
#include "CN.h"
#include "Node.h"

template<>
const int                   CN<int>::Immense = 0x80000;

class Search {
	int kiki[36 * 5];
	unsigned long long calCount = 0;
	unsigned long long lastCalCount = 0;
	int maxDepth;

	HashMap hashP;
	HashMap hashE;

	string mode;

	unsigned int calPnDnCount = 0;

	bool overFlag = false;

	unsigned int enemyBlue;

	bool needans = false;
	int answerID[50];
	bool needReadableAns = false;
	bool needpndn = false;

	bool pnend = false;
	bool dnend = false;

	vector<Node*> vecNode;

	CN<int> maxrootpn;
	CN<int> maxrootdn;

public:
	string answerBoard;
	string subMode;

	Search(string mode, string subMode) {	//ゲームが始まる前の処理
		this->mode = mode;
		this->subMode = subMode;

		vecNode.reserve(64000000);

		maxrootpn = CN<int>(0);
		maxrootdn = CN<int>(0);

		int y, x, dir, i, j;
		int dy[4] = { -1, 0, 1, 0 };
		int dx[4] = { 0, 1, 0, -1 };

		for (i = 0; i < 180; i++) kiki[i] = -1;
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				i = y * 6 + x;
				j = 0;
				for (dir = 0; dir < 4; dir++) {
					int ny = y + dy[dir];
					int nx = x + dx[dir];
					if (0 <= ny && ny < 6 && 0 <= nx && nx < 6) {
						kiki[5 * i + j] = ny * 6 + nx;
						j++;
					}
				}
			}
		}
	}

	// 問題を解く
	int think(string board, int eB, int maxDepth, int minDepth=0) {
		this->maxDepth = maxDepth;
		BitBoard bb;
		bb.toBitBoard(board);

		BitBoard emptybb;
		emptybb.toBitBoard("................................................");

		int i = (mode == "n") ? 2 : 3;
		if (minDepth != 0) i = minDepth;
		int space = 2;

		enemyBlue = eB;
		calCount = 0;
		while (i <= maxDepth) {
			//cout << "Depth " << i << endl;
			lastCalCount = 0;
			hashP.clear();
			hashE.clear();
			nodeCount = 1;

			Node* emptyn = new Node;
			emptyn->initNode(emptybb, 0, 0);
			vecNode.push_back(emptyn);

			Node* n = new Node;
			n->initNode(bb, 0, 1);
			vecNode.push_back(n);

			maxrootpn = 0;
			maxrootdn = 0;
			pnend = false;
			dnend = false;

			CN<int> pn, dn;
			pn = pn - 1;
			dn = dn - 1;

			MIDOr(*n, pn, dn, 0, i, eB);

			if (pn <= 0) {
				if (needans == true) answerBoard = returnSequenceBoard(i);
				releaseVec(vecNode);
				return ((mode == "n") ? (i + 1) : i);
			}
			releaseVec(vecNode);

			if (overFlag == true) {
				cout << mode << subMode << ", " << bitCount(bb.existB) << bitCount(bb.existR)
					<< eB << (bitCount(bb.existP | bb.existEB | bb.existER) - eB)
					<< ", " << board
					<< ": over" << endl;
				overFlag = false;
				break;
			}

			i += space;
		}
		return 0;
	}

	// 解答例の連続した盤面を戻す
	string returnSequenceBoard(int actionNum) {
		string board_sequence;
		string solution_action;
		string previous_board = vecNode[1]->bb.returnString();
		int move_id = 1;
		SearchAnswer(actionNum);
		for (int i = 0; i < 50; i++) {
			if (answerID[i] == 0) break;
			Node* _n;
			_n = new Node;
			*_n = *vecNode[answerID[i]];
			for (int grid_id = 0; grid_id < 36; grid_id += 6) {
				if (needReadableAns) { board_sequence.append(to_string(6 - grid_id / 6) + " "); }
				board_sequence.append(_n->bb.returnString().substr(grid_id, 6));
				if (needReadableAns) { board_sequence.append("\n"); }
			}
			if (needReadableAns) { board_sequence.append("  abcdef\n"); }
			board_sequence.append("\n");
			if (needReadableAns) {
				string current_board = _n->bb.returnString();
				int src_id = 36;
				int dst_id = 36;
				char moved_piece;
				int grid_id = 0;
				while (grid_id < 36) {
					char p = previous_board[grid_id];
					if (current_board[grid_id] == p) {
						grid_id++;
						continue;
					}
					if (((i % 2 == 0) && (p == 'B' || p == 'R'))
						|| ((i % 2 == 1) && (p == 'b' || p == 'r' || p == 'u')))
					{
						src_id = grid_id;
						moved_piece = p;
						break;
					}
					grid_id++;
				}
				grid_id = 0;
				while (grid_id < 36) {
					char p = current_board[grid_id];
					if (p != previous_board[grid_id] && p == moved_piece) {
						dst_id = grid_id;
						break;
					}
					grid_id++;
				}
				char src_x = char(src_id % 6 + 'a');
				char src_y = char((6 - src_id / 6) + '0');
				string direction;
				if (dst_id - src_id == -6) { direction = "up"; }
				else if (dst_id - src_id == 6) { direction = "down"; }
				else if (dst_id - src_id == -1) { direction = "left"; }
				else if (dst_id - src_id == 1) { direction = "right"; }
				else { direction = "ERROR"; }
				solution_action += (to_string(move_id) + ". " + string(1, moved_piece));
				solution_action += (" " + string(1, src_x) + string(1, src_y) + " " + direction + "\n");
				previous_board = current_board;
				move_id++;
			}
			delete _n;
		}
		if (needReadableAns && actionNum % 2 == 0) {
			// 脱出
			solution_action += (to_string(move_id) + ". B");
			if (previous_board[0] == 'B') {
				solution_action += " a6 left\n";
			} else if (previous_board[5] == 'B') {
				solution_action += " f6 right\n";
			}
		}
		return board_sequence + (needReadableAns ? solution_action : "");
	}

	unsigned long long returnCount(void) { return calCount; }		// 計算ノード数を返す
	unsigned long long returnLastCount(void) { return lastCalCount; }// 必勝手数の計算ノード数を返す
	CN<int> returnMaxPn(void) { return maxrootpn; }			// ルートノードの最大証明数を返す
	CN<int> returnMaxDn(void) { return maxrootdn; }			// ルートノードの最大反証数を返す
	void setNeedAns(bool ans) { this->needans = ans; }		// 解答例が必要かどうかを切り替え．デフォルトではオフ
	void setNeedReadableAns(bool ans) { this->needReadableAns = ans; } // 解答例に改行を入れるかどうか．デフォルトではオフ
	void setNeedPnDn(bool pndn) { this->needpndn = pndn; }	// ルートノードの最大証明数反証数の計算をするか切り替え．デフォルトではオフ

private:

	// 解答を探す
	void SearchAnswer(int actionNum) {
		int turn = 0;
		int id = 1;
		int depth = 0;
		int _depthToGoalAnd;
		int _depthToGoalOr;
		for (int a = 0; a < 50; a++) {
			answerID[a] = 0;
		}

		//cout << "searchStart" << endl;
		CalDepthToGoal(1, 0, 0, actionNum);
		//cout << "selectStart" << endl;

		while (1) {
			_depthToGoalAnd = 100;
			_depthToGoalOr = -1;

			Node* n;
			n = new Node;
			*n = *vecNode[id];
			for (int i = 0; i < 32; i++) {
				if (n->childID[i] == 0) break;
				Node nChild = *vecNode[n->childID[i]];
				if (nChild.bb.existR == 0ll) { continue; }
				if (nChild.dn.isinf() == true) {
					if (turn == 0 && nChild.depthToGoal < _depthToGoalAnd) {
						_depthToGoalAnd = nChild.depthToGoal;
						answerID[depth] = n->childID[i];
						id = n->childID[i];
					}
					else if (turn == 1 && nChild.depthToGoal > _depthToGoalOr) {
						Search* s;
						s = new Search(mode, subMode);
						bool is_skip =
							(s->think(nChild.bb.returnString(), enemyBlue, nChild.depthToGoal - 1) != 0);
						delete s;
						if (is_skip) { continue; }
						_depthToGoalOr = nChild.depthToGoal;
						answerID[depth] = n->childID[i];
						id = n->childID[i];
					}
				}
			}
			delete n;
			if (answerID[depth] == 0) break;
			depth += 1;
			turn = 1 - turn;
		}
	}

	// 各ノードの詰みまでの最短手数を調べる
	void CalDepthToGoal(int id, int depth, int turn, int actionNum) {
		int _depthToGoal;
		if (turn == 0) _depthToGoal = 300;
		else _depthToGoal = -300;

		Node* n;
		n = new Node;
		*n = *vecNode[id];

		int i = 0;

		for (i = 0; i < 32; i++) {
			if (n->childID[i] == 0) break;
			if (vecNode[n->childID[i]]->dn.isinf() == true) {
				CalDepthToGoal(n->childID[i], depth + 1, 1 - turn, actionNum);
				Node* nChild;
				nChild = new Node;
				*nChild = *vecNode[n->childID[i]];
				if (turn == 0 && nChild->depthToGoal != -1 && nChild->depthToGoal <= _depthToGoal) {
					_depthToGoal = nChild->depthToGoal;
					vecNode[id]->depthToGoal = nChild->depthToGoal + 1;

				}
				else if (turn == 1 && nChild->depthToGoal >= _depthToGoal && nChild->depthToGoal != -1) {
					_depthToGoal = nChild->depthToGoal;
					vecNode[id]->depthToGoal = nChild->depthToGoal + 1;
				}
				else if (turn == 1 && nChild->depthToGoal == -1) {
					_depthToGoal = 100;
					vecNode[id]->depthToGoal = -1;
				}
				delete nChild;
			}
			else if (turn == 1) {
				_depthToGoal = 100;
				vecNode[id]->depthToGoal = -1;
			}
		}
		if (i == 0 && (n->bb.check(turn, enemyBlue, "n") == 0 || n->bb.check(turn, enemyBlue, "n") == 2)) {
			vecNode[id]->depthToGoal = 0;
		}
		delete n;
	}

	// ルートノードの最大証明数と反証数を計算
	void calRootPnDn(int maxdepth) {
		if (maxrootpn.isinf() == false && pnend == false) {
			CN<int> rootpn = calPn(1, 0, maxdepth);
			if (rootpn.isinf() == true)	pnend = true;
			else if (maxrootpn < rootpn) maxrootpn = rootpn;
		}
		if (maxrootdn.isinf() == false && dnend == false) {
			CN<int> rootdn = calDn(1, 0, maxdepth);
			if (rootdn.isinf() == true)	dnend = true;
			else if (maxrootdn < rootdn) maxrootdn = rootdn;
		}
	}

	// ルートノードの証明数を計算
	CN<int> calPn(unsigned int id, int depth, int maxdepth) {
		CN<int> minimum;
		CN<int> sum(0);
		Node _n = *vecNode[id];

		for (int i = 0; i < 32; i++) {
			if (_n.childID[i] == 0) break;
			if (depth % 2 == 0) {
				if (minimum <= maxrootpn) break;
				if (vecNode[_n.childID[i]] == NULL) {}
				else if (vecNode[_n.childID[i]]->expanded == true && depth < maxDepth) {
					minimum = min(calPn(_n.childID[i], depth + 1, maxdepth), minimum);
				}
				else minimum = min(vecNode[_n.childID[i]]->pn, minimum);
			}
			else {
				if (sum.isinf() == true) break;
				if (vecNode[_n.childID[i]] == NULL) sum += CN<int>();
				else if (vecNode[_n.childID[i]]->expanded == true && depth < maxDepth) {
					sum += calPn(_n.childID[i], depth + 1, maxdepth);
				}
				else sum += vecNode[_n.childID[i]]->pn;
			}
		}
		if (depth % 2 == 0) {
			return minimum;
		}
		else {
			return sum;
		}
	}

	// ルートノードの反証数を計算
	CN<int> calDn(unsigned int id, int depth, int maxdepth) {
		CN<int> minimum;
		CN<int> sum(0);

		Node _n = *vecNode[id];

		for (int i = 0; i < 32; i++) {
			if (_n.childID[i] == 0) break;

			if (depth % 2 == 1) {
				if (minimum <= maxrootdn) break;
				if (vecNode[_n.childID[i]] == NULL) minimum = 0;
				else if (vecNode[_n.childID[i]]->expanded == true && depth < maxdepth) {
					minimum = min(calDn(_n.childID[i], depth + 1, maxdepth), minimum);
				}
				else minimum = min(vecNode[_n.childID[i]]->dn, minimum);
			}
			else {
				if (sum.isinf() == true) break;
				if (vecNode[_n.childID[i]] == NULL) {}
				else if (vecNode[_n.childID[i]]->expanded == true && depth < maxdepth) {
					sum += calDn(_n.childID[i], depth + 1, maxdepth);
				}
				else sum += vecNode[_n.childID[i]]->dn;
			}
		}
		if (depth % 2 == 1) return minimum;
		else return sum;
	}

	CN<int> DeltaMinOr(Node& n) {
		CN<int> minimum;

		for (int i = 0; i < 32; i++) {
			if (n.childID[i] == 0) break;
			if (vecNode[n.childID[i]] == NULL) {
				minimum = 0;
			}
			else {
				minimum = min(vecNode[n.childID[i]]->dn, minimum);
			}
		}
		return minimum;
	}

	CN<int> DeltaMinAnd(Node& n) {
		CN<int> minimum;

		for (int i = 0; i < 32; i++) {
			if (n.childID[i] == 0) break;
			if (vecNode[n.childID[i]] == NULL) {}
			else {
				minimum = min(vecNode[n.childID[i]]->pn, minimum);
			}
		}
		return minimum;
	}

	CN<int> PhiSumOr(Node& n) {
		CN<int> sum(0);

		for (int i = 0; i < 32; i++) {
			if (n.childID[i] == 0) break;
			if (vecNode[n.childID[i]] == NULL) {
				sum += CN<int>();
			}
			else {
				sum += vecNode[n.childID[i]]->pn;
			}
		}
		return sum;
	}

	CN<int> PhiSumAnd(Node& n) {
		CN<int> sum(0);

		for (int i = 0; i < 32; i++) {
			if (n.childID[i] == 0) break;
			if (vecNode[n.childID[i]] == NULL) {}
			else {
				sum += vecNode[n.childID[i]]->dn;
			}
		}
		return sum;
	}

	int SelectChildOr(Node& n, CN<int>& pn, CN<int>& dn2) {
		CN<int> dn1;
		int best = 0;

		pn.infinity();

		for (int i = 0; i < 32; ++i) {
			if (n.childID[i] == 0) break;

			Node _n;
			if (vecNode[n.childID[i]] == NULL) {
				_n.pn = CN<int>();
				_n.dn = CN<int>(0);
			}
			else {
				_n = *vecNode[n.childID[i]];
			}

			if (_n.dn < dn1) {
				best = i;
				pn = _n.pn;
				dn2 = dn1;
				dn1 = _n.dn;
			}
			else if (_n.dn < dn2) {
				dn2 = _n.dn;
			}
		}
		return best;
	}

	int SelectChildAnd(Node& n, CN<int>& dn, CN<int>& pn2) {
		CN<int> pn1;
		int best = 0;

		dn.infinity();

		for (int i = 0; i < 32; ++i) {
			if (n.childID[i] == 0) break;

			Node _n;
			if (vecNode[n.childID[i]] == NULL) {
				_n.pn = CN<int>();
				_n.dn = CN<int>(0);
			}
			else {
				_n = *vecNode[n.childID[i]];
			}

			if (_n.pn < pn1) {
				best = i;
				dn = _n.dn;
				pn2 = pn1;
				pn1 = _n.pn;
			}
			else if (_n.pn < pn2) {
				pn2 = _n.pn;
			}
		}
		return best;
	}

	void MIDOr(Node& n, CN<int>& pn, CN<int>& dn, int depth, int maxdepth, int enemyBlue) {
		calCount += 1;
		lastCalCount += 1;
		if (overFlag == true) {
			return;
		}

		if (vecNode.size() >= 56000000) {
			overFlag = true;
			return;
		}

		// 1.応手生成
		switch (n.bb.check(0, enemyBlue, subMode)) {
		case 0:
			n.pn = 0;
			n.dn.infinity();
			pn = 0;
			dn.infinity();
			PutInHash(hashP, n.bb, n.depth, &n);

			return;
		case 1:
			n.pn.infinity();
			n.dn = 0;
			pn.infinity();
			dn = 0;
			PutInHash(hashP, n.bb, n.depth, &n);
			return;
		case 2:
			n.pn = 0;
			n.dn.infinity();
			pn = 0;
			dn.infinity();
			PutInHash(hashP, n.bb, n.depth, &n);

			return;
		default:
			int escapeMaxDepth = (maxdepth % 2 == 0) ? maxdepth : (maxdepth - 1);
			bool isMoreThanMinEscape = ((n.bb.minEscapeAction() * 2) > (escapeMaxDepth - depth));
			bool canPrune = ((mode == "n" && isMoreThanMinEscape)
				|| (mode == "p" && isMoreThanMinEscape && ((n.bb.existER == 0)
					|| (n.bb.minCaptureAction() > (maxdepth - depth)))));
			if ((depth == maxdepth) || canPrune) {
				n.pn.infinity();
				n.dn = 0;
				pn.infinity();
				dn = 0;
				PutInHash(hashP, n.bb, n.depth, &n);
				return;
			}
			break;
		}

		int from[32], to[32], moveNum;
		if (n.expanded == false) {
			moveNum = n.bb.makeMoves(0, kiki, from, to);
			n.bb.moveOrder(0, moveNum, from, to);
			makeChild(hashE, &n, from, to, moveNum, maxdepth, vecNode);

			calPnDnCount++;
			if ((maxdepth <= 7 || (maxdepth <= 11 && calPnDnCount % 100 == 0) || (maxdepth <= 17 && calPnDnCount % 1000 == 0) || calPnDnCount % 10000 == 0) && this->needpndn == true) {
				calRootPnDn(maxdepth);
			}
		}

		// 2.ハッシュによるサイクル回避
		n.setPnDn(CN<int>(), CN<int>(0));
		PutInHash(hashP, n.bb, n.depth, &n);

		CN<int> dmin, psum;
		CN<int> _pn, _dn;

		// 3.多重反復深化
		while (1) {

			dmin = DeltaMinAnd(n);
			psum = PhiSumAnd(n);

			if (pn <= dmin || dn <= psum) {
				pn = dmin;
				dn = psum;
				n.setPnDn(dmin, psum);
				PutInHash(hashP, n.bb, n.depth, &n);

				return;
			}
			CN<int> _pn2;
			int best = SelectChildAnd(n, _dn, _pn2);

			Node* nextN = vecNode[n.childID[best]];
			_dn = (dn - psum) + _dn;
			_pn = min(pn, _pn2 + CN<int>(1));

			MIDAnd(*nextN, _pn, _dn, depth + 1, maxdepth, enemyBlue);

			if (overFlag == true) {
				return;
			}
		}
	}

	void MIDAnd(Node& n, CN<int>& pn, CN<int>& dn, int depth, int maxdepth, int enemyBlue) {
		calCount += 1;
		lastCalCount += 1;
		if (overFlag == true) {
			return;
		}
		if (vecNode.size() >= 56000000) {
			overFlag = true;
			return;
		}

		// 1.応手生成
		switch (n.bb.check(1, enemyBlue, subMode)) {
		case 0:
			n.pn = 0;
			n.dn.infinity();
			pn = 0;
			dn.infinity();
			PutInHash(hashE, n.bb, n.depth, &n);
			return;
		case 1:
			n.pn.infinity();
			n.dn = 0;
			pn.infinity();
			dn = 0;
			PutInHash(hashE, n.bb, n.depth, &n);
			return;
		case 3:
			n.pn.infinity();
			n.dn = 0;
			pn.infinity();
			dn = 0;
			PutInHash(hashE, n.bb, n.depth, &n);
			return;
		default:
			int escapeMaxDepth = (maxdepth % 2 == 0) ? maxdepth : (maxdepth - 1);
			bool isMoreThanMinEscape = ((n.bb.minEscapeAction() * 2 + 1) > (escapeMaxDepth - depth));
			bool canPrune = ((mode == "n" && isMoreThanMinEscape)
				|| (mode == "p" && isMoreThanMinEscape && ((n.bb.existER == 0)
					|| (n.bb.minCaptureAction() > (maxdepth - depth)))));
			if ((depth == maxdepth) || canPrune) {
				n.pn.infinity();
				n.dn = 0;
				pn.infinity();
				dn = 0;
				PutInHash(hashE, n.bb, n.depth, &n);
				return;
			}
			break;
		}

		int from[32], to[32], moveNum;
		if (n.expanded == false) {
			moveNum = n.bb.makeMoves(1, kiki, from, to);
			n.bb.moveOrder(1, moveNum, from, to);
			makeChild(hashP, &n, from, to, moveNum, maxdepth, vecNode);
		}

		// 2.ハッシュによるサイクル回避
		n.setPnDn(CN<int>(), CN<int>(0));
		PutInHash(hashE, n.bb, n.depth, &n);

		CN<int> dmin, psum;
		CN<int> _pn, _dn;

		// 3.多重反復深化
		while (1) {
			dmin = DeltaMinOr(n);
			psum = PhiSumOr(n);

			if (dn <= dmin || pn <= psum) {
				dn = dmin;
				pn = psum;
				n.setPnDn(psum, dmin);
				PutInHash(hashE, n.bb, n.depth, &n);
				return;
			}

			CN<int> _dn2;
			int best = SelectChildOr(n, _pn, _dn2);

			Node* nextN = vecNode[n.childID[best]];
			_pn = (pn - psum) + _pn;
			_dn = min(dn, _dn2 + CN<int>(1));

			MIDOr(*nextN, _pn, _dn, depth + 1, maxdepth, enemyBlue);

			if (overFlag == true) {
				return;
			}
		}
	}
};
