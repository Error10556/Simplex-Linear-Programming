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

void printMatrix(const fmatrix& mat)
{
    int h = mat.Height();
    int w = mat.Width();
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
            cout << mat.Cell(i, j) << '\t';
        cout << endl;
    }
    cout << endl;
}

State Simplex(fmatrix& mat, double eps, vector<int>& basic)
{
	double* toprow = mat.RowPtr(0);
	int w = mat.Width();
	int h = mat.Height();
	double* fracs = new double[h - 1];
	basic.resize(h - 1);
	for (int i = 0; i < basic.size(); i++)
		basic[i] = i + h - 1;
    //printMatrix(mat);
	while (true)
	{
		int col = GetBest(toprow, toprow + w - 1, less<double>()) - toprow;
		if (toprow[col] > -eps)
			break;
		for (int i = 0; i < h - 1; i++)
        {
            auto sol = mat.Cell(1 + i, w - 1);
            auto divisor = mat.Cell(1 + i, col);
            if (sol >= 0 && sol <= eps && divisor < 0)
                fracs[i] = -1;
            else
			    fracs[i] = sol / divisor;
        }
        int row = GetBest(fracs, fracs + h - 1, less<double>(), [eps](double a)-> bool {return (/*isfinite(a) ||*/ a <= 1e9) && a >= 0;}) - fracs;
		if (row == h - 1) {
			delete[] fracs;
			return NOTAPPLICABLE;
		}
		basic[row] = col;
		row++;
        //cout << "Row: " << row << ", col: " << col << endl;
		NormalizeRow(mat, row, col);
		for (int i = 0; i < h; i++)
		{
			if (i == row)
				continue;
			AddRow(mat, row, i, -mat.Cell(i, col));
		}
        auto solcell = mat.Cell(0, w - 1);
		if (!isfinite(solcell) || solcell >= 1e9) {
			delete[] fracs;
			return UNBOUNDED;
		}
        //printMatrix(mat);
	}
	delete[] fracs;
	return SOLVED;
}

vector<double> ReadVector(int n)
{
	vector<double> res(n);
	for (double& i : res)
		cin >> i;
	return res;
}

void printRow(const vector<double>& v, int n) {
	for (int i = 0; i < n; ++i) {
        if (v[i] < 0) cout << " - ";
        if (v[i] > 0 && i > 0) cout << " + ";
        if (v[i] != 0) cout << abs(v[i]) << " * x" << (i + 1);
    }
}
void printOptimizationProblem(const vector<double>& obj, const fmatrix& mat) {
    cout << "Objective function: ";
    cout << "max z = ";
    printRow(obj, obj.size());
    
    cout << "\nSubject to the constraints:\n";
    for (int i = 1; i < mat.Height(); ++i) {
		printRow(mat.GetRow(i), obj.size());
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
	State solver_state = Simplex(mat, eps, basic);

	if (solver_state != SOLVED) {
		cout << "The method is not applicable!\n";
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
