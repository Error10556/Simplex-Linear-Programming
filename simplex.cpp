#include <iostream>
#include <vector>
#include <cmath>
#include "Matrix.h"
#include <sstream>
#define DEFAULT_EPS 0.000001

enum State {
    UNBOUNDED,
    NOTAPPLICABLE,
    SOLVED
};

using namespace std;

State solver_state = SOLVED;

typedef Matrix<double> fmatrix;

void NormalizeRow(fmatrix& mat, int row, int col)
{
	double factor = mat.Cell(row, col);
	double* prow = mat.RowPtr(row);
	int w = mat.Width();
	for (int i = 0; i < w; i++)
		if (i == col)
			prow[i] = 1;
		else
			prow[i] /= factor;
}

void AddRow(fmatrix& mat, int src, int dest, double factor)
{
	double* psrc = mat.RowPtr(src);
	double* pdest = mat.RowPtr(dest);
	int w = mat.Width();
	for (int i = 0; i < w; i++)
		pdest[i] += factor * psrc[i];
}

template<class Iter, class Comp, class Pred>
Iter GetBest(Iter begin, Iter end, Comp comparator, Pred filter)
{
	Iter i = begin;
	Iter best = end;
	for (; i != end; ++i)
	{
		if (!filter(*i))
			continue;
		if (best == end || comparator(*i, *best))
			best = i;
	}
	return best;
}

template<class Iter, class Comp>
Iter GetBest(Iter begin, Iter end, Comp comparator)
{
	return GetBest(begin, end, comparator, [](decltype(*begin)) {return true;});
}

bool Simplex(fmatrix& mat, double eps, vector<int>& basic)
{
	double* toprow = mat.RowPtr(0);
	int w = mat.Width();
	int h = mat.Height();
	double* fracs = new double[h - 1];
	basic.resize(h - 1);
	for (int i = 0; i < basic.size(); i++)
		basic[i] = i + h - 1;
	while (true)
	{
		int col = GetBest(toprow, toprow + w - 1, less<double>()) - toprow;
		if (toprow[col] > -eps)
			break;
		for (int i = 0; i < h - 1; i++)
			fracs[i] = mat.Cell(1 + i, w - 1) / mat.Cell(1 + i, col);
		int row = GetBest(fracs, fracs + h - 1, less<double>(), [](double a)-> bool {return isfinite(a) && a >= 0;}) - fracs;
		if (row == h - 1) {
			delete[] fracs;
			return false;
		}
		basic[row] = col;
		row++;
		NormalizeRow(mat, row, col);
		for (int i = 0; i < h; i++)
		{
			if (i == row)
				continue;
			AddRow(mat, row, i, -mat.Cell(i, col));
		}
		if (isinf(mat.Cell(0, w - 1))) {
			solver_state = UNBOUNDED;
			return true;
		}
	}
	delete[] fracs;
	return true;
}

vector<double> ReadVector(int n)
{
	vector<double> res(n);
	for (double& i : res)
		cin >> i;
	return res;
}

void printOptimizationProblem(const vector<double>& obj, const fmatrix& mat) {
    cout << "Objective function: ";
    cout << "max z = ";
    
    for (int i = 0; i < obj.size(); ++i) {
        if (obj[i] < 0) cout << " - ";
        if (obj[i] > 0 && i > 0) cout << " + ";
        cout << abs(obj[i]) << " * x" << (i + 1);
    }
    
    cout << "\nSubject to the constraints:\n";
    for (int i = 1; i < mat.Height(); ++i) {
        for (int j = 0; j < mat.Width() - 1; ++j) {
            if (obj[j] < 0) cout << " - ";
        	if (obj[j] > 0 && j > 0) cout << " + ";
            cout << abs(mat.Cell(i, j)) << " * x" << (j + 1);
        }
        cout << " <= " << mat.Cell(i, mat.Width() - 1) << "\n";
    }
}

int main()
{
	vector<double> obj;
    string input;
    cout << "Enter a vector of coefficients of the objective function: ";
    getline(cin, input);
    stringstream ss(input);
    double number;
    while (ss >> number) {
        obj.push_back(number);
    }
	int nvars = obj.size();

	cout << "Enter the number of constraints: ";
	int nconstraints; cin >> nconstraints;
	fmatrix mat(1 + nconstraints, nvars + nconstraints + 1);
	int w = mat.Width();
	for (int i = 0; i < nvars; i++)
		mat.Cell(0, i) = -obj[i];

	cout << "Enter a matrix of coefficients of the constraint functions\n";
	for (int i = 1; i <= nconstraints; i++)
	{
		double* row = mat.RowPtr(i);
		for (int j = 0; j < nvars; j++)
			cin >> row[j];
		row[nvars + i - 1] = 1;
	}

    cout << "Enter a vector of right-hand side values: ";
    for (int i = 1; i <= nconstraints; i++)
        cin >> mat.Cell(i, w - 1);

	cout << "Enter approximation accuracy (optional): ";
	cin.ignore();
    getline(cin, input);
    double eps;
    if (input.empty()) eps = DEFAULT_EPS;
    else eps = stod(input);

	printOptimizationProblem(obj, mat);

	vector<int> basic;
	bool isSimplexApplicable = Simplex(mat, eps, basic);

	if (!isSimplexApplicable || solver_state == UNBOUNDED) {
		cout << "The method is not applicable!";
		return 0;
	}

	vector<double> vals(nvars);
	for (int i = 0; i < basic.size(); i++)
		if (basic[i] < nvars)
			vals[basic[i]] = mat.Cell(i + 1, w - 1);

	cout << "The vector of decision variables: (";
	for (int i = 0; i < nvars; i++) {
		cout << vals[i];
		if (i < nvars - 1) cout << ", ";
	}
	cout << ")\n";
	cout << "Maximum value of objective function: " << mat.Cell(0, w - 1) << "\n";
}
