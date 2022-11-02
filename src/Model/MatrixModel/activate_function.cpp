#include "activate_function.h"
namespace s21 {
void ActivateFunction::Use(std::vector<double>& value, int n) {
    for (int i = 0; i < n; i++) value[i] = 1 / (1 + exp(-value[i]));
}
void ActivateFunction::UseDer(std::vector<double>& value, int n) {
    for (int i = 0; i < n; i++) value[i] = value[i] * (1 - value[i]);
}

double ActivateFunction::UseDer(double value) {
    value = 1 / (1 + exp(-value));
    return value;
}
}  // namespace s21
