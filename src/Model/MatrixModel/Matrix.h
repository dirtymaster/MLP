#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <vector>
namespace s21 {
class Matrix {
public:
    void Init(int rows, int cols);
    void Rand();
    static void Multi(const Matrix& m, const std::vector<double>& b, int n, std::vector<double>& c);
    static void Multi_T(const Matrix& m, const std::vector<double>& b, int n, std::vector<double>& c);
    static void SumVector(std::vector<double>& a, const std::vector<double>& b, int n);
    double& operator()(int i, int j);
    static double GenerateRandomNumber(double min, double max);
    int GetRows();
    int GetCols();

private:
    std::vector<std::vector<double>> matrix;
    int rows, cols;
};
}  // namespace s21

#endif  // MATRIX_H_
