#pragma once
#include <vector>

template<class T>
class Matrix
{
public:
	using CalcType = T;
	using ConstRef = const Matrix<T>&;
	using MoveRef = Matrix<T>&&;
	using ValueType = Matrix<T>;
	using RefType = ValueType&;
private:
	int n, m;
	CalcType* arr;
public:
	int BufSize() const;
	CalcType* Data();
	const CalcType* Data() const;
	Matrix(int n, int m);
	Matrix(ConstRef other);
	Matrix(MoveRef other);
	RefType operator=(ConstRef other);
	RefType operator=(MoveRef other);
	int Height() const;
	int Width() const;
	CalcType& Cell(int row, int col);
	CalcType Cell(int row, int col) const;
	std::vector<CalcType> GetRow(int row) const;
	CalcType* RowPtr(int row);
	const CalcType* RowPtr(int row) const;
	~Matrix();
};
