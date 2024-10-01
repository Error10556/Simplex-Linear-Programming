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
		cout << "Row " << row << "; col " << col << '\n';
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

int main()
{
	vector<double> obj;
    string input;
    cout << "Enter the vector of coefficients of objective function: ";
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
	cout << "In the next " << nconstraints << " lines, enter " << nvars <<
        " numbers - the coefficients of the constraint.\n";
	for (int i = 1; i <= nconstraints; i++)
	{
		double* row = mat.RowPtr(i);
		for (int j = 0; j < nvars; j++)
			cin >> row[j];
		row[nvars + i - 1] = 1;
	}
    cout << "Enter a line with right-hand side numbers: ";
    for (int i = 1; i <= nconstraints; i++)
        cin >> mat.Cell(i, w - 1);
	double eps;
	cout << "Epsilon: "; cin >> eps;
	vector<int> basic;
	bool isSimplexApplicable = Simplex(mat, eps, basic);
	if (!isSimplexApplicable) {
		cout << "The method is not applicable!";
		return;
	}
	vector<double> vals(nvars);
	for (int i = 0; i < basic.size(); i++)
		if (basic[i] < nvars)
			vals[basic[i]] = mat.Cell(i + 1, w - 1);

	cout << "The vector of decision variables: (";
	for (int i = 0; i < nvars; i++)
		cout << vals[i] << ', ';
	cout << ")\n";
	cout << "Maximum value of objective function: " << mat.Cell(0, w - 1);
}
