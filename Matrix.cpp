#include "Matrix.h"
#include <cmath>
#include <cstring>
using namespace std;
template<class T>

int Matrix<T>::BufSize() const
{
	return sizeof(T) * n * m;
}

template<class T>
typename Matrix<T>::CalcType* Matrix<T>::Data()
{
	return arr;
}

template<class T>
const typename Matrix<T>::CalcType* Matrix<T>::Data() const
{
	return arr;
}

template <class T>
Matrix<T>::Matrix(int n, int m) : n(n), m(m) {
	arr = new T[n * m]{};
}

template<class T>
Matrix<T>::Matrix(ConstRef other) : Matrix(other.n, other.m)
{
	memcpy(arr, other.arr, BufSize());
}

template<class T>
Matrix<T>::Matrix(MoveRef other) : Matrix(1, 1)
{
	swap(n, other.n);
	swap(m, other.m);
	swap(arr, other.arr);
}

template<class T>
typename Matrix<T>::RefType Matrix<T>::operator=(ConstRef other)
{
	bool realloc = n * m != other.m * other.n;
	n = other.n;
	m = other.m;
	if (realloc) {
		delete[] arr;
		arr = new T[n * m];
	}
	memcpy(arr, other.arr, BufSize());
	return *this;
}

template<class T>
typename Matrix<T>::RefType Matrix<T>::operator=(MoveRef other)
{
	swap(n, other.n);
	swap(m, other.m);
	swap(arr, other.arr);
	return *this;
}

template<class T>
int Matrix<T>::Height() const
{
	return n;
}

template<class T>
int Matrix<T>::Width() const
{
	return m;
}

template<class T>
typename Matrix<T>::CalcType& Matrix<T>::Cell(int row, int col)
{
	return arr[m * row + col];
}

template<class T>
typename Matrix<T>::CalcType Matrix<T>::Cell(int row, int col) const
{
	return arr[m * row + col];
}

template<class T>
std::vector<typename Matrix<T>::CalcType> Matrix<T>::GetRow(int row) const
{
	auto ptr = RowPtr(row);
	return vector<CalcType>(ptr, ptr + m);
}

template<class T>
typename Matrix<T>::CalcType* Matrix<T>::RowPtr(int row)
{
	return arr + row * m;
}

template<class T>
const typename Matrix<T>::CalcType* Matrix<T>::RowPtr(int row) const
{
	return arr + row * m;
}

template<class T>
Matrix<T>::~Matrix()
{
	delete[] arr;
}

template class Matrix<float>;
template class Matrix<double>;

