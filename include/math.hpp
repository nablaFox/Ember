#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

template <typename T, std::size_t Rows, std::size_t Cols>
class Mat {
public:
	std::array<T, Rows * Cols> elements{};

	Mat() = default;

	Mat(std::initializer_list<std::initializer_list<T>> list) {
		assert(list.size() == Rows && "Incorrect number of rows");

		auto it = elements.begin();

		for (auto row : list) {
			assert(row.size() == Cols && "Incorrect number of cols");
			for (auto val : row) {
				*it++ = val;
			}
		}
	}

	T& operator()(std::size_t row, std::size_t col) {
		return elements[row * Cols + col];
	}

	const T& operator()(std::size_t row, std::size_t col) const {
		return elements[row * Cols + col];
	}

	void print() const {
		for (std::size_t i = 0; i < Rows; i++) {
			std::cout << "[";
			for (std::size_t j = 0; j < Cols; j++) {
				std::cout << std::setw(8) << std::fixed << std::setprecision(4)
						  << (*this)(i, j) << " ";
			}

			std::cout << "]\n";
		}
	}

	static Mat identity() {
		static_assert(Rows == Cols, "Identity must be square");
		Mat matrix{};

		for (std::size_t i = 0; i < Rows; i++) {
			matrix(i, i) = T(1);
		}

		return matrix;
	}

	Mat<T, Cols, Rows> transpose() {
		Mat<T, Cols, Rows> transposed{};

		for (std::size_t i = 0; i < Rows; i++) {
			for (std::size_t j = 0; j < Cols; j++) {
				transposed(j, i) = (*this)(i, j);
			}
		}

		return transposed;
	}

	Mat& operator+=(const Mat& other) {
		for (std::size_t i = 0; i < Rows * Cols; i++) {
			elements[i] += other.elements[i];
		}

		return *this;
	}

	Mat& operator*=(float scalar) {
		for (std::size_t i = 0; i < Rows * Cols; i++) {
			elements[i] *= scalar;
		}

		return *this;
	}

	Mat& operator/=(float scalar) {
		for (std::size_t i = 0; i < Rows * Cols; i++) {
			elements[i] /= scalar;
		}

		return *this;
	}

	Mat operator+(const Mat& other) const {
		Mat matrix = *this;

		return matrix += other;
	}

	Mat operator*(float scalar) const {
		Mat matrix = *this;

		return matrix *= scalar;
	}

	Mat operator/(float scalar) const {
		Mat matrix = *this;

		return matrix /= scalar;
	}
};

template <typename T, std::size_t Rows>
class Vec : public Mat<T, Rows, 1> {
public:
	Vec() = default;

	Vec(std::initializer_list<T> list) {
		assert(list.size() == Rows && "Incorrect number of elements");
		std::copy(list.begin(), list.end(), this->elements.begin());
	}

	Vec(const Mat<T, Rows, 1>& m) {
		std::copy(m.elements.begin(), m.elements.end(), this->elements.begin());
	}

	T& operator[](std::size_t row) { return (*this)(row, 0); }
	const T& operator[](std::size_t row) const { return (*this)(row, 0); }

	float length() const {
		float sum = 0;

		for (std::size_t i = 0; i < Rows; i++) {
			sum += (*this)[i] * (*this)[i];
		}

		return std::sqrt(sum);
	}

	Vec& normalize() {
		float len = length();

		for (std::size_t i = 0; i < Rows; i++) {
			(*this)[i] /= len;
		}

		return *this;
	}
};

template <typename T, std::size_t Rows, std::size_t N, std::size_t Cols>
Mat<T, Rows, Cols> operator*(const Mat<T, Rows, N>& a, const Mat<T, N, Cols>& b) {
	Mat<T, Rows, Cols> result;

	for (std::size_t i = 0; i < Cols; i++) {
		for (std::size_t j = 0; j < Rows; j++) {
			T res = T(0);

			for (std::size_t k = 0; k < N; k++) {
				res += a(j, k) * b(k, i);
			}

			result(j, i) = res;
		}
	}

	return result;
}

using Mat4 = Mat<float, 4, 4>;
using Mat3 = Mat<float, 3, 3>;
using Vec3 = Vec<float, 3>;
using Vec4 = Vec<float, 4>;
