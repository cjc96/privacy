#include "prefix.h"

prefixnode::prefixnode(vector<pair<int, int>> idx1, int depth1, vector<int> path1) : idx(idx1), depth(depth1), path(path1) {
	score = idx.size();
}

prefixnode::~prefixnode() {
}

prefix::prefix(engine& eng1) : tree(eng1) {
}

prefix::~prefix() {
}

void prefix::load(vector<vector<int>>& data1, int fanout1) {
	data = data1;
	nodes.clear();

	vector<pair<int, int>> idx;
	for (int i = 0; i < data.size(); i++)
		for (int l = 0; l < data[i].size(); l++)
			idx.push_back(make_pair(i, l));

	nodes.push_back(prefixnode(idx, 0, vector<int>()));
	fanout = fanout1;
	realtree(idx.size() * 0.005);					//control the tree size; this is a tradeoff between time and memory
}

void prefix::assign_content(int idx, double epsilon) {
}

bool prefix::expandable(int idx) {
	return true;
}

vector<prefixnode> prefix::split(int idx) {
	prefixnode& f = nodes[idx];
	vector<prefixnode> children;

	vector<vector<pair<int, int>>> sub(fanout);
	for (pair<int, int> i : f.idx) {
		if (f.depth + i.second < data[i.first].size()) sub[data[i.first][f.depth + i.second]].push_back(i);
	}

	for (int i = 0; i < fanout; i++) {
		vector<int> newpath = f.path;
		newpath.push_back(i);											
		children.push_back(prefixnode(sub[i], f.depth + 1, newpath));
	}
	return children;
}

int prefix::size() {
	return 0;
}

int prefix::count(vector<int>& q) {
	int c = 0;
	while (!nodes[c].leaf && nodes[c].depth < q.size()) c = nodes[c].next[q[nodes[c].depth]];

	if (nodes[c].depth == q.size()) return nodes[c].idx.size();
	else {
		int sum = 0;
		for (pair<int, int> i : nodes[c].idx) sum += is_prefix(q, i);
		return sum;
	}
}

bool prefix::is_prefix(vector<int>& pre, pair<int, int>& pos) {
	if (data[pos.first].size() - pos.second < pre.size()) return false;
	return pre == vector<int>(data[pos.first].begin() + pos.second, data[pos.first].begin() + pos.second + pre.size());
}