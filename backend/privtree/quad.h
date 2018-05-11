#pragma once
using namespace std;

#include"dual.h"

class quad : public tree<spatialnode>
{
public:
	quad(engine&, string, int);
	~quad();

	void assign_content(int, double);
	bool expandable(int);
	vector<spatialnode> split(int);
	int size();

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
