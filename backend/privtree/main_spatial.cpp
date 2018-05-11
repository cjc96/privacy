#include<iostream>
#include<fstream>
#include<ctime>
#include<string>
using namespace std;

#include"dual.h"
#include"quad.h"
#include"noise.h"

void ptime(time_t& pre, string s) {
	cout << s << ": " << difftime(time(0), pre) << endl;
	pre = time(0);
}

template <typename T>
double mean_relative_error(const vector<double>& a, vector<T>& q, double smooth) {
	double err = 0.0;
	for (int i = 0; i < a.size(); i++)
		err += abs(q[i].ans - a[i]) / max(q[i].ans, smooth);			// smoothed relative error
	return err / a.size();												// mean relative error
}

double cus_vmin0,cus_vmin1,cus_vmax0,cus_vmax1,cus_eps;

int main(int argc, char *argv[]) {
	string filename = argv[1];
	// double para = atof(argv[2]);
	// int rep = atoi(argv[3]);
	// string id = argv[4];
	cus_vmin0 = atof(argv[2]);
	cus_vmin1 = atof(argv[3]);
	cus_vmax0 = atof(argv[4]);
	cus_vmax1 = atof(argv[5]);
	cus_eps = atof(argv[6]);
	// cout << cus_vmin0 << " " << cus_vmin1 << " " << cus_vmax0 << " " << cus_vmax1 << " " << cus_eps << endl;
	time_t tic = time(0);

	int dim = 2;
	// if (filename == "gowalla.dat") dim = 2;
	// else if (filename == "storage.dat") dim = 2;
	// else if (filename == "landmark.dat") dim = 2;
	// else if (filename == "road.dat") dim = 2;
	// else if (filename == "goall.dat") dim = 2;
	// else if (filename == "nyc.dat") dim = 2;
	// else if (filename == "beijing.dat") dim = 2;

	// ofstream log("test.log");
	random_device rd;						//non-deterministic random engine
	engine eng(rd());						//deterministic engine with a random seed

	dual dual(eng, filename, dim);
	quad quad(eng, filename, dim);
	double smooth = 0.001 * dual.size();

	// ptime(tic, "data loaded");
	// cout << filename << "\t" << dual.size() << endl;

	dual.magic(0.5, 0.0, 10.0);				//build a tree in advance for efficient query generation
	vector<vector<spatialquery> > queries;
	for (double scale : {0.0001}) 
		queries.push_back(dual.build_query(1, scale));

	// ptime(tic, "query loaded");
    // cout << "size: " << queries.size() << endl;
	// auto j = queries[0][0];
	// for (auto tmp : j.pmin) {
	// 	cout << "min:" << tmp << endl;
	// }
	// for (auto tmp : j.pmax) {
	// 	cout << "max:" << tmp << endl;
	// }
	// cout << "ans " << j.ans << endl;
	// cout << "dis" << j.dis() << endl;
	// 
	// for (double epsilon : {0.05, 0.1, 0.2, 0.4, 0.8, 1.6}) {
	// 	vector<double> err(queries.size());
	// 	for (int i = 0; i < rep; i++) {
	// 		dual.magic(0.5, 0.0, epsilon);
	// 		for (int qi = 0; qi < err.size(); qi++) err[qi] += mean_relative_error(dual.count(queries[qi]), queries[qi], smooth);
	// 	}
	// 	for (int qi = 0; qi < err.size(); qi++) log << err[qi] / rep << "\t";
	// 	log << endl;
	// }

	// for (double epsilon : {0.05, 0.1, 0.2, 0.4, 0.8, 1.6}) {
	// 	vector<double> err(queries.size());
	// 	for (int i = 0; i < rep; i++) {
	// 		quad.magic(0.5, 0.0, epsilon);
	// 		for (int qi = 0; qi < err.size(); qi++) err[qi] += mean_relative_error(quad.count(queries[qi]), queries[qi], smooth);
	// 	}
	// 	for (int qi = 0; qi < err.size(); qi++) log << err[qi] / rep << "\t";
	// 	log << endl;
	// }

	// log.close();
}
