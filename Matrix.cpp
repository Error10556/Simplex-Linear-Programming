#include "Matrix.h"
#include <string>
#include <cmath>
#include <cstring>
using namespace std;

MatrixException::MatrixException(const string& message) : msg(message) {}

string MatrixException::Message() const
{
	return msg;
}

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
bool Matrix<T>::operator==(ConstRef other) const noexcept
{
	return n == other.n && m == other.m && !memcmp(arr, other.arr, BufSize());
}

template<class T>
bool Matrix<T>::operator!=(ConstRef other) const noexcept
{
	return !(*this == other);
}

template<class T>
static string MatrixSizeRepr(typename Matrix<T>::ConstRef mat)
{
	return "(" + to_string(mat.Height()) + "x" + to_string(mat.Width()) + ")";
}

template<class T>
typename Matrix<T>::ValueType Matrix<T>::operator*(ConstRef other) const
{
	if (m != other.n) throw MatrixException(("Matrix multiplication: "
		+ MatrixSizeRepr<T>(*this) + " * " + MatrixSizeRepr<T>(other)).c_str());
	ValueType res(n, other.m);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < other.m; j++)
		{
			CalcType& dest = res.Cell(i, j);
			for (int k = 0; k < m; k++)
				dest += Cell(i, k) * other.Cell(k, j);
		}
	return res;
}

template<class T>
typename Matrix<T>::ValueType Matrix<T>::operator*(CalcType other) const
{
	Matrix<T> res = *this;
	res *= other;
	return res;
}

template<class T>
typename Matrix<T>::ValueType Matrix<T>::operator+(ConstRef other) const
{
	auto res = *this;
	res += other;
	return res;
}

template<class T>
typename Matrix<T>::ValueType Matrix<T>::operator-(ConstRef other) const
{
	auto res = *this;
	res -= other;
	return res;
}

template<class T>
typename Matrix<T>::ValueType Matrix<T>::operator-() const
{
	auto res = *this;
	res *= -1;
	return res;
}

template<class T>
typename Matrix<T>::RefType Matrix<T>::operator+=(ConstRef other)
{
	if (n != other.n || m != other.m) throw MatrixException(("Matrix addition: "
		+ MatrixSizeRepr<T>(*this) + " + " + MatrixSizeRepr<T>(other)).c_str());
	T* dest = arr;
	const T* src = other.arr;
	for (int i = n * m; i; i--, dest++, src++)
		*dest += *src;
	return *this;
}

template<class T>
typename Matrix<T>::RefType Matrix<T>::operator-=(ConstRef other)
{
	if (n != other.n || m != other.m) throw MatrixException(("Matrix subtraction: "
		+ MatrixSizeRepr<T>(*this) + " - " + MatrixSizeRepr<T>(other)).c_str());
	T* dest = arr;
	const T* src = other.arr;
	for (int i = n * m; i; i--, dest++, src++)
		*dest -= *src;
	return *this;
}

template<class T>
typename Matrix<T>::RefType Matrix<T>::operator*=(CalcType other)
{
	T* dest = arr;
	for (int i = n * m; i; i--, dest++)
		*dest *= other;
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
typename Matrix<T>::CalcType Matrix<T>::Abs() const
{
	CalcType res = 0;
	CalcType* p = arr;
	for (int i = n * m; i; i--, p++)
		res += *p * *p;
	return sqrt(res);
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

template<class T>
T Logistic(T x)
{
	if (x < -10) return 0;
	if (x > 10) return 1;
	return 1 / (1 + exp(x));
}

template class Matrix<float>;
template class Matrix<double>;

