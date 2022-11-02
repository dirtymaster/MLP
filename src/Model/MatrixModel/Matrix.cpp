#include "Matrix.h"
namespace s21 {
void Matrix::Init(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    matrix.resize(rows);
    for (int i = 0; i < rows; i++) matrix[i].resize(cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }
}

double Matrix::GenerateRandomNumber(double min, double max) {
    double random = (double)rand() / RAND_MAX;
    return min + random * (max - min);
}

void Matrix::Rand() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = GenerateRandomNumber(-1.0, 1.0);
        }
    }
}

void Matrix::Multi(const Matrix& m1, const std::vector<double>& neuron, int n, std::vector<double>& c) {
    if (m1.cols != n) throw std::invalid_argument("Error Multi \n");

    for (int i = 0; i < m1.rows; ++i) {
        double tmp = 0;
        for (int j = 0; j < m1.cols; ++j) {
            tmp += m1.matrix[i][j] * neuron[j];
        }
        c[i] = tmp;
    }
}

void Matrix::Multi_T(const Matrix& m1, const std::vector<double>& neuron, int n, std::vector<double>& c) {
    if (m1.rows != n) throw std::invalid_argument("Error Multi \n");

    for (int i = 0; i < m1.cols; ++i) {
        double tmp = 0;
        for (int j = 0; j < m1.rows; ++j) {
            tmp += m1.matrix[j][i] * neuron[j];
        }
        c[i] = tmp;
    }
}
double& Matrix::operator()(int i, int j) { return matrix[i][j]; }

void Matrix::SumVector(std::vector<double>& a, const std::vector<double>& b, int n) {
    for (int i = 0; i < n; i++) a[i] += b[i];
}

int Matrix::GetRows() { return rows; }

int Matrix::GetCols() { return cols; }
}  // namespace s21
