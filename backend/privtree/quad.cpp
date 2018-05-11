#include "quad.h"

quad::quad(engine& eng, string filename, int dim1) : tree(eng), dim(dim1) {
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
	fanout = pow(2, dim);
	sens = 1;
}

quad::~quad() {
}

bool quad::expandable(int idx) {
	return true;
}

vector<spatialnode> quad::split(int idx) {
	spatialnode& f = nodes[idx];
	vector<spatialnode> children;

	vector<double> mid;
	for (int t = 0; t < dim; t++)
		mid.push_back((f.pmin[t] + f.pmax[t]) / 2.0);
	vector<vector<int>> div(fanout);

	for (int i : f.idx) {
		int index = 0;
		for (int t = 0; t < dim; t++) index = index * 2 + (data[i][t] > mid[t]);
		div[index].push_back(i);
	}

	for (int index = 0; index < fanout; index++) {
		int tmp = index;
		vector<double> vmin(dim), vmax(dim);
		for (int t = dim - 1; t > -1; t--) {
			if (tmp % 2) {
				vmin[t] = mid[t];
				vmax[t] = f.pmax[t];
			}
			else {
				vmin[t] = f.pmin[t];
				vmax[t] = mid[t];
			}
			tmp /= 2;
		}
		children.push_back(spatialnode(div[index], f.depth + 1, vmin, vmax));
	}
	return children;
}

void quad::assign_content(int idx, double epsilon) {
	spatialnode& c = nodes[idx];
	if (c.leaf) {
		c.content = c.idx.size() + 1.0 / epsilon * noise::nextLaplace(eng);
	}
	else {
		c.content = 0.0;
		for (int child : c.next) {
			assign_content(child, epsilon);
			c.content += nodes[child].content;
		}
	}
}

int quad::size() {
	return data.size();
}

vector<double> quad::count(vector<spatialquery>& queries) {
	vector<double> answers;
	for (spatialquery& q : queries) answers.push_back(max(0.0, noisycount(0, q)));
	return answers;
}

double quad::noisycount(int idx, spatialquery& q) {
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

int quad::realcount(int idx, spatialquery& q) {
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



bool quad::within(vector<double>& p, vector<double>& pmin, vector<double>& pmax) {
	for (int t = 0; t < dim; t++)
		if (p[t] < pmin[t] || p[t] > pmax[t]) return false;
	return true;
}

double quad::intersect(vector<double>& amin, vector<double>& amax, vector<double>& bmin, vector<double>& bmax) {
	double inter = 1.0;
	for (int t = 0; t < dim; t++)
		inter *= max(0.0, min(amax[t], bmax[t]) - max(amin[t], bmin[t]));
	return inter;
}

double quad::area(vector<double>& pmin, vector<double>& pmax) {
	double area = 1.0;
	for (int t = 0; t < dim; t++)
		area *= pmax[t] - pmin[t];
	return area;
}

