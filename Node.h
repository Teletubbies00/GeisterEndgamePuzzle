#pragma once
#include "IncludeLib.h"
#include "CN.h"
#include "HashTable.h"

struct Node {
	// 拡張したか
	bool expanded = false;

	unsigned int id;

	CN<int> pn, dn;

	int depth;

	BitBoard bb;

	unsigned int childID[32] = {};

	int depthToGoal = -1;

	void initNode(BitBoard bb, int depth, int ID) {
		this->pn = 1;
		this->dn = 1;

		this->expanded = false;

		this->bb = bb;

		this->depth = depth;

		this->id = ID;
	}

	void setPnDn(CN<int> pn, CN<int> dn) {
		this->pn = pn;
		this->dn = dn;
	}
};

// 子ノードを作る
bool makeChild(HashMap& map, Node* n, int from[], int to[], int moveNum, int maxDepth, vector<Node*>& vecNode) {

	if (n->expanded == false) {
		for (int i = 0; i < moveNum; i++) {
			BitBoard bbtmp = n->bb;
			n->bb.move(from[i], to[i]);
			Node* cnp = LookUpHash(map, n->bb, n->depth + 1);

			// ハッシュに子ノードと同じ盤面があった，新規に追加せず
			if (cnp != NULL) {
				n->childID[i] = cnp->id;
				if (n->childID[i] == -1) {
					cout << "error" << endl;
				}
			}
			else {
				// なかった．新規に追加する
				Node* child = NULL;
				child = new Node;
				nodeCount += 1;
				child->initNode(n->bb, n->depth + 1, nodeCount);
				n->childID[i] = nodeCount;
				vecNode.push_back(child);
				PutInHash(map, child->bb, child->depth, child);
			}
			n->bb = bbtmp;
		}
		n->expanded = true;
	}
	return false;
}

void releaseVec(vector<Node*>& vecNode) {

	for (int i = 0; i < vecNode.size(); i++) {
		if (vecNode[i] != NULL) {
			delete vecNode[i];
			vecNode[i] = NULL;
		}
	}
	vecNode.clear();
}
