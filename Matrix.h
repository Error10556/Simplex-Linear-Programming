// Code by Timur Usmanov, 2024
#pragma once
#include <vector>
#include <stdexcept>
#include <string>

class MatrixException : public std::exception {
private:
	std::string msg;
public:
	MatrixException(const std::string& s);
	std::string Message() const;
};

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
	bool operator==(ConstRef other) const noexcept;
	bool operator!=(ConstRef other) const noexcept;
	ValueType operator*(ConstRef other) const;
	ValueType operator*(CalcType other) const;
	ValueType operator+(ConstRef other) const;
	ValueType operator-(ConstRef other) const;
	ValueType operator-() const;
	RefType operator+=(ConstRef other);
	RefType operator-=(ConstRef other);
	RefType operator*=(CalcType other);
	int Height() const;
	int Width() const;
	CalcType Abs() const;
	CalcType& Cell(int row, int col);
	CalcType Cell(int row, int col) const;
	std::vector<CalcType> GetRow(int row) const;
	CalcType* RowPtr(int row);
	const CalcType* RowPtr(int row) const;
	~Matrix();
};
