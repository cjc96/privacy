#pragma once
#include<vector>
#include<fstream>
#include<algorithm>
using namespace std;

#include"noise.h"

template <typename T>
class tree
{
public:
	tree(engine& eng1) : eng(eng1) {}
	~tree() {}

	void realtree(double theta) {
		vector<int> priv(1);
		int i = 0;
		while (i < priv.size()) {
			int idx = priv[i];
			if (expandable(idx) && score(idx) > theta) {
				vector<int> children = expand(idx);
				priv.insert(priv.end(), children.begin(), children.end());
			}
			else nodes[idx].leaf = true;

			i++;
		}
	}

	void naive(double ratio, double theta, double ep, int height) {
		ep = ep / sens;													// normalized by sensitivity
		double ep1 = ratio * ep, ep2 = ep - ep1;						// build the tree with ep1 and materialize noisy entities with ep2
		double lambda = height / ep1;
		theta = max(log(fanout) * lambda, theta);						// convergency

		vector<int> priv(1);
		int i = 0;
		while (i < priv.size()) {
			int idx = priv[i];

			if (depth(idx) < height && expandable(idx) && score(idx) + lambda * noise::nextLaplace(eng) > theta) {
				vector<int> children = expand(idx);
				priv.insert(priv.end(), children.begin(), children.end());
			}
			else nodes[idx].leaf = true;

			i++;
		}
		assign_content(0, ep2);
	}

	void magic(double ratio, double theta, double ep) {			
		ep = ep / sens;													// normalized by sensitivity
		double ep1 = ratio * ep, ep2 = ep - ep1;						// build the tree with ep1 and materialize noisy entities with ep2
		double d = log(fanout);											// expectation = 2
		double lambda = (double)(2 * fanout - 1) / (fanout - 1) / ep1;

		vector<int> priv(1);
		int i = 0;
		while (i < priv.size()) {
			int idx = priv[i];
			double tscore = max(score(idx) - depth(idx) * d * lambda, theta - d * lambda);

			if (expandable(idx) && tscore + lambda * noise::nextLaplace(eng) > theta) {
				vector<int> children = expand(idx);
				priv.insert(priv.end(), children.begin(), children.end());
			}
			else nodes[idx].leaf = true;

			i++;
		}
		assign_content(0, ep2);
	}

	double score(int idx) {
		return nodes[idx].score;
	}

	int depth(int idx) {
		return nodes[idx].depth;
	}

	vector<int> expand(int idx) {
		if (nodes[idx].next.empty()) {
			vector<T> children = split(idx);
			for (T child : children) {
				nodes[idx].next.push_back(nodes.size());
				nodes.push_back(child);
			}
		}
		nodes[idx].leaf = false;
		return nodes[idx].next;
	}

	void reset() {								//free memory
		T root = nodes[0];
		root.next.clear();
		nodes.swap(vector<T>());
		nodes.push_back(root);
	}

	int stat(int idx, vector<int>& scores) {
		T& c = nodes[idx];
		if (c.leaf) {
			scores[min((int)scores.size() - 1, (int)c.idx.size())]++;
			return 1;
		}
		else {
			int sum = 0;
			for (int child : c.next) sum += stat(child, scores);
			return sum;
		}
	}

	// virtual functions
	virtual void assign_content(int, double) = 0;
	virtual bool expandable(int) = 0;
	virtual vector<T> split(int) = 0;
	virtual int size() = 0;

	vector<T> nodes;
	engine& eng;
	int fanout;
	int sens;
	double e = 2.718281828;
};

