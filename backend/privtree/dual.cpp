#include "dual.h"

spatialnode::spatialnode(vector<int>& idx1, int depth1, vector<double>& pmin1, vector<double>& pmax1) 
	: idx(idx1), depth(depth1), pmin(pmin1), pmax(pmax1) {
	score = idx.size();
}

spatialnode::~spatialnode() {
}


spatialquery::spatialquery(double ans1, vector<double>& pmin1, vector<double>& pmax1) 
	: ans(ans1), pmin(pmin1), pmax(pmax1) {
}

spatialquery::~spatialquery() {
}


dual::dual(engine& eng, string filename, int dim1) : tree(eng), dim(dim1) {
	ifstream tfile(filename);
	vector<double> vmin(dim, 1e10), vmax(dim, -1e10);
	vector<int> idx;
	vector<double> newpoint;

	double x;
	while (tfile >> x) {
		newpoint.push_back(x);
		if (newpoint.size() == dim) {
			data.push_back(newpoint);
			idx.push_back(idx.size());
			for (int t = 0; t < dim; t++) {
				if (newpoint[t] < vmin[t]) vmin[t] = newpoint[t];
				if (newpoint[t] > vmax[t]) vmax[t] = newpoint[t];
			}
			newpoint.clear();
		}
	}

	for (int t = 0; t < dim; t++) {								// expand a little bit
		vmin[t] += -1e-6;
		vmax[t] += 1e-6;
	}
	nodes.push_back(spatialnode(idx, 0, vmin, vmax));			
	tfile.close();
	fanout = 2;
	sens = 1;
}

dual::~dual() {
}

bool dual::expandable(int idx) {
	return true;
}

vector<spatialnode> dual::split(int idx) {
	spatialnode& f = nodes[idx];
	vector<spatialnode> children;

	int cut = f.depth % dim;
	double mid = (f.pmin[cut] + f.pmax[cut]) / 2.0;
	vector<int> d0, d1;

	for (int i : f.idx) {
		if (data[i][cut] < mid) d0.push_back(i);
		else d1.push_back(i);
	}

	vector<double> pmid = f.pmax; pmid[cut] = mid;
	children.push_back(spatialnode(d0, f.depth + 1, f.pmin, pmid));
	pmid = f.pmin; pmid[cut] = mid;
	children.push_back(spatialnode(d1, f.depth + 1, pmid, f.pmax));

	return children;
}
extern double cus_vmin0,cus_vmin1,cus_vmax0,cus_vmax1,cus_eps;
void dual::assign_content(int idx, double epsilon) {
	spatialnode& c = nodes[idx];
	if (c.leaf) {
		c.content = c.idx.size() + 1.0 / cus_eps * noise::nextLaplace(eng);
	}
	else {
		c.content = 0.0;
		for (int child : c.next) {
			assign_content(child, epsilon);
			c.content += nodes[child].content;
		}
	}
}

int dual::size() {
	return data.size();
}

vector<spatialquery> dual::build_query(int n, double init_scale) {
	vector<spatialquery> queries;
	vector<double>& pmin = nodes[0].pmin;
	vector<double>& pmax = nodes[0].pmax;

	while (queries.size() < n) {
		double scale = pow(noise::nextDouble(eng, init_scale, init_scale * 10), 1.0 / dim);
		vector<double> vmin, vmax;
		for (int t = 0; t < dim; t++) {
			double length = (pmax[t] - pmin[t]) * scale;
			vmin.push_back(noise::nextDouble(eng, pmin[t], pmax[t] - length));
			vmax.push_back(vmin.back() + length);
		}

		vmin[0] = cus_vmin0;
		vmin[1] = cus_vmin1;
		vmax[0] = cus_vmax0;
		vmax[1] = cus_vmax1;
		// cout << "vmin: " << vmin[0] << " " << vmin[1] << " " << vmin[2] << " " << vmin[3] << endl;
		// cout << "vmax: " << vmax[0] << " " << vmax[1] << " " << vmax[2] << " " << vmax[3] << endl;
		auto xxxx=spatialquery(0.0, vmin, vmax);
		double ans = realcount(0, xxxx);
		ofstream log("return.json");
		log << "{ \"realAns\": " << ans << ", ";
		queries.push_back(spatialquery(ans, vmin, vmax));
		ans = noisycount(0, xxxx);
		log << "\"noisyAns\": " << ans << " }"  << endl;
		log.close();
	}
	return queries;
}

vector<double> dual::count(vector<spatialquery>& queries) {
	vector<double> answers;
	for (spatialquery& q : queries) answers.push_back(max(0.0, noisycount(0, q)));
	return answers;
}

double spatialquery::dis() {
	return this->ans * 0.15;
}

double dual::noisycount(int idx, spatialquery& q) {
	spatialnode& c = nodes[idx];
	double inter = intersect(c.pmin, c.pmax, q.pmin, q.pmax);
	
	if (inter == 0.0) return 0.0;
	else if (inter == area(c.pmin, c.pmax)) return c.content;				//full coverage
	else if (c.leaf) return  inter / area(c.pmin, c.pmax) * c.content;
	else {
		double sum = 0.0;
		for (int child : c.next) sum += noisycount(child, q);
		return sum;
	}
}

int dual::realcount(int idx, spatialquery& q) {
	spatialnode& c = nodes[idx];
	double inter = intersect(c.pmin, c.pmax, q.pmin, q.pmax);

	int sum = 0;
	if (inter == 0.0) return 0;
	else if (inter == area(c.pmin, c.pmax)) return c.idx.size();			//full coverage
	else if (c.next.empty()) {												//no child, exhaustive search
		for (int i : c.idx) sum += within(data[i], q.pmin, q.pmax);
	}
	else {																	//split into children nodes
		for (int child : c.next) sum += realcount(child, q);
	}
	return sum;
}


bool dual::within(vector<double>& p, vector<double>& pmin, vector<double>& pmax) {
	for (int t = 0; t < dim; t++)
		if (p[t] < pmin[t] || p[t] > pmax[t]) return false;
	return true;
}

double dual::intersect(vector<double>& amin, vector<double>& amax, vector<double>& bmin, vector<double>& bmax) {
	double inter = 1.0;
	for (int t = 0; t < dim; t++)
		inter *= max(0.0, min(amax[t], bmax[t]) - max(amin[t], bmin[t]));
	return inter;
}

double dual::area(vector<double>& pmin, vector<double>& pmax) {
	double area = 1.0;
	for (int t = 0; t < dim; t++)
		area *= pmax[t] - pmin[t];
	return area;
}