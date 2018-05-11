#pragma once
#include<vector>
#include<string>
#include<deque>
using namespace std;

#include"tree.h"

class prefixnode
{
public:
	prefixnode(vector<pair<int, int>>, int, vector<int>);
	~prefixnode();

	vector<pair<int, int>> idx;
	vector<int> path;
	vector<int> next;
	int depth;
	bool leaf;
	double score;
};

class prefix : public tree<prefixnode>
{
public:
	prefix(engine&);
	~prefix();
	void load(vector<vector<int>>&, int);

	void assign_content(int, double);
	bool expandable(int);
	vector<prefixnode> split(int);
	int size();

	int count(vector<int>&);
	bool is_prefix(vector<int>&, pair<int, int>&);

	vector<vector<int>> data;
};

