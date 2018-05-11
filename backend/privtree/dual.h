#pragma once

#include"tree.h"
#include<iostream>

class spatialnode
{
public:
	spatialnode(vector<int>&, int, vector<double>&, vector<double>&);
	~spatialnode();

	vector<int> idx;
	vector<int> next;
	int depth;
	vector<double> pmin, pmax;

	bool leaf;					// if a leaf
	double score;				// score for tree construction
	double content;				// content for query answering, only available if leaf
};


class spatialquery
{
public:
	spatialquery(double, vector<double>&, vector<double>&);
	~spatialquery();

	double ans;
	double dis();
	vector<double> pmin, pmax;
};


class dual : public tree<spatialnode>
{
public:
	dual(engine&, string, int);
	~dual();

	void assign_content(int, double);
	bool expandable(int);
	vector<spatialnode> split(int);
	int size();

	vector<spatialquery> build_query(int, double);				//with scale
	vector<double> count(vector<spatialquery>&);
	double noisycount(int, spatialquery&);
	int realcount(int, spatialquery&);

	//tools
	bool within(vector<double>&, vector<double>&, vector<double>&);
	double intersect(vector<double>&, vector<double>&, vector<double>&, vector<double>&);
	double area(vector<double>&, vector<double>&);
	//end of tools

	vector<vector<double>> data;
	int dim;
};
