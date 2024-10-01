#include <iostream>
#include <vector>
#include <cmath>
#include "Matrix.h"
#include <sstream>
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

void Simplex(fmatrix& mat, double eps, vector<int>& basic)
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
		if (row == h - 1)
			break;
		basic[row] = col;
		row++;
		NormalizeRow(mat, row, col);
		for (int i = 0; i < h; i++)
		{
			if (i == row)
				continue;
			AddRow(mat, row, i, -mat.Cell(i, col));
		}
		cout << "Row " << row << "; col " << col << '\n';
	}
	delete[] fracs;
}

vector<double> ReadVector(int n)
{
	vector<double> res(n);
	for (double& i : res)
		cin >> i;
	return res;
}

int main()
{
	vector<double> obj;
    string input;
    cout << "Coefficients of objective function: ";
    getline(cin, input);
    stringstream ss(input);
    double number;
    while (ss >> number) {
        obj.push_back(number);
    }
	int nvars = obj.size();
	cout << "Number of constraints: ";
	int nconstraints; cin >> nconstraints;
	fmatrix mat(1 + nconstraints, nvars + nconstraints + 1);
	int w = mat.Width();
	for (int i = 0; i < nvars; i++)
		mat.Cell(0, i) = -obj[i];
	cout << "In the next " << nconstraints << " lines, enter " << nvars + 1 << " numbers: the coefficients and the limit.\n";
	for (int i = 1; i <= nconstraints; i++)
	{
		double* row = mat.RowPtr(i);
		for (int j = 0; j < nvars; j++)
			cin >> row[j];
		cin >> row[w - 1];
		row[nvars + i - 1] = 1;
	}
	double eps;
	cout << "Epsilon: "; cin >> eps;
	vector<int> basic;
	Simplex(mat, eps, basic);
	vector<double> vals(nvars);
	for (int i = 0; i < basic.size(); i++)
		if (basic[i] < nvars)
			vals[basic[i]] = mat.Cell(i + 1, w - 1);

	for (int i = 0; i < nvars; i++)
		cout << "x" << i + 1 << " = " << vals[i] << '\n';
	cout << "z = " << mat.Cell(0, w - 1) << '\n';
}
